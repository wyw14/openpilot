#!/usr/bin/env python3
"""Read-only comparison of the two openpilot release file rule sets.

This command applies, against the *same* working tree:
  1. the zipapp rules from pack.py (DIRS walk + EXTS extension allow list +
     EXCLUDE path-substring list, implemented by pack.py's copy()), and
  2. the release_files.py rules (`git ls-files --recurse-submodules` plus a
     blacklist/whitelist and the INCLUDE_BIG_MODEL big-model skip).

For every path it reports which side includes or excludes it and why. It also
reports untracked/ignored files, symlinks and submodules explicitly.

Hard guarantees:
  * no archive is created (zipapp.create_archive is never called),
  * no packaging target module is imported (pack.py's importlib.import_module
    is not replicated) -- the rules are parsed out of pack.py/release_files.py
    with ast instead, so there is no import-time side effect and no rule drift,
  * the working tree is never modified (only read-only filesystem walks and
    read-only git commands are used).

Paths are printed POSIX-style, relative to the repo root, sorted once and in a
single stable order. The zipapp EXCLUDE test is a plain substring match on the
source path (``exc in src`` in pack.py); pack.py is built on Linux, so using
forward-slash relative paths reproduces its behavior exactly.
"""

from __future__ import annotations

import argparse
import ast
import dataclasses
import json
import os
import re
import subprocess
import sys
import types
from pathlib import Path

HERE = Path(__file__).resolve().parent
DEFAULT_ROOT = HERE.parents[1]
PACK_PY = HERE / "pack.py"
RELEASE_FILES_PY = HERE / "release_files.py"
BIG_MODEL_ENV = "INCLUDE_BIG_MODEL"


class RuleSourceError(RuntimeError):
  pass


class GitError(RuntimeError):
  pass


# ----------------------------------------------------------------------------
# Rule loading (ast only: importing pack.py would import openpilot and importing
# a zipapp target module is forbidden; release_files.py is parsed too so this
# command works without the openpilot package on sys.path).
# ----------------------------------------------------------------------------


@dataclasses.dataclass(frozen=True)
class ZipappRules:
  dirs: dict[str, str]  # source dir (repo-relative) -> package name in the zipapp
  exts: tuple[str, ...]  # pack.py EXTS, suffix allow list
  exclude: tuple[str, ...]  # pack.py EXCLUDE, plain substring patterns


@dataclasses.dataclass(frozen=True)
class ReleaseRules:
  blacklist: tuple[str, ...]
  whitelist: tuple[str, ...]
  big_model_prefix: str  # startswith() prefix skipped unless INCLUDE_BIG_MODEL


def _literal_assign(tree: ast.Module, name: str, source: Path):
  for node in tree.body:
    if isinstance(node, ast.Assign) and any(isinstance(t, ast.Name) and t.id == name for t in node.targets):
      return ast.literal_eval(node.value)
  raise RuleSourceError(f"{source}: could not find literal assignment '{name}'")


def load_zipapp_rules(path: Path = PACK_PY) -> ZipappRules:
  tree = ast.parse(path.read_text(encoding="utf-8"), filename=str(path))
  dirs = _literal_assign(tree, "DIRS", path)
  exts = _literal_assign(tree, "EXTS", path)
  exclude = _literal_assign(tree, "EXCLUDE", path)
  if not (isinstance(dirs, dict) and all(isinstance(k, str) and isinstance(v, str) for k, v in dirs.items()) and dirs):
    raise RuleSourceError(f"{path}: DIRS must be a non-empty dict[str, str]")
  if not (isinstance(exts, (list, tuple)) and all(isinstance(e, str) for e in exts) and exts):
    raise RuleSourceError(f"{path}: EXTS must be a non-empty list[str]")
  if not (isinstance(exclude, (list, tuple)) and all(isinstance(e, str) for e in exclude)):
    raise RuleSourceError(f"{path}: EXCLUDE must be a list[str]")
  return ZipappRules(dirs=dict(dirs), exts=tuple(exts), exclude=tuple(exclude))


def _whitelist_assign_node(tree: ast.Module, source: Path) -> ast.expr:
  for node in tree.body:
    target = None
    if isinstance(node, ast.AnnAssign) and isinstance(node.target, ast.Name):
      target = node.target.id
      value = node.value
    elif isinstance(node, ast.Assign):
      target = next((t.id for t in node.targets if isinstance(t, ast.Name)), None)
      value = node.value
    else:
      continue
    if target == "whitelist":
      if value is None:
        raise RuleSourceError(f"{source}: 'whitelist' has no value")
      return value
  raise RuleSourceError(f"{source}: could not find 'whitelist' assignment")


def load_release_rules(path: Path = RELEASE_FILES_PY, include_big_model: bool = False) -> ReleaseRules:
  tree = ast.parse(path.read_text(encoding="utf-8"), filename=str(path))
  blacklist = _literal_assign(tree, "blacklist", path)
  if not (isinstance(blacklist, (list, tuple)) and all(isinstance(p, str) for p in blacklist)):
    raise RuleSourceError(f"{path}: blacklist must be a list[str]")
  for pattern in blacklist:
    try:
      re.compile(pattern)
    except re.error as e:
      raise RuleSourceError(f"{path}: invalid blacklist regex {pattern!r}: {e}") from None

  # Evaluate release_files.py's own whitelist expression verbatim, with only a
  # stubbed os.getenv in scope -- no openpilot imports, no subprocess.
  def fake_getenv(key, default=None):
    return "1" if include_big_model and key == BIG_MODEL_ENV else default

  sandbox = {"os": types.SimpleNamespace(getenv=fake_getenv)}
  whitelist = eval(  # noqa: S307 - AST node extracted from the repo's own release_files.py
    compile(ast.Expression(_whitelist_assign_node(tree, path)), str(path), "eval"), sandbox, {}
  )
  if not (isinstance(whitelist, (list, tuple)) and all(isinstance(p, str) for p in whitelist)):
    raise RuleSourceError(f"{path}: whitelist must evaluate to list[str]")
  for pattern in whitelist:
    try:
      re.compile(pattern)
    except re.error as e:
      raise RuleSourceError(f"{path}: invalid whitelist regex {pattern!r}: {e}") from None

  # The big-model skip is inside release_files.py's __main__ block; pull the
  # exact startswith() constant from the source instead of restating it here.
  prefixes = {
    n.args[0].value
    for n in ast.walk(tree)
    if isinstance(n, ast.Call)
    and isinstance(n.func, ast.Attribute)
    and n.func.attr == "startswith"
    and n.args
    and isinstance(n.args[0], ast.Constant)
    and isinstance(n.args[0].value, str)
  }
  if len(prefixes) != 1:
    raise RuleSourceError(f"{path}: expected exactly one startswith() string constant, found {sorted(prefixes)!r}")
  return ReleaseRules(tuple(blacklist), tuple(whitelist), prefixes.pop())


# ----------------------------------------------------------------------------
# Read-only git access
# ----------------------------------------------------------------------------


def _git(root: Path, args: list[str]) -> subprocess.CompletedProcess:
  return subprocess.run(["git", "-C", str(root), *args], capture_output=True)


def git_output(root: Path, args: list[str]) -> bytes:
  proc = _git(root, args)
  if proc.returncode != 0:
    raise GitError(f"git {' '.join(args)} failed in {root}:\n{proc.stderr.decode('utf-8', 'replace').strip()}")
  return proc.stdout


def git_stdout_lines(root: Path, args: list[str]) -> tuple[list[str], str]:
  proc = _git(root, args)
  if proc.returncode != 0:
    raise GitError(f"git {' '.join(args)} failed in {root}:\n{proc.stderr.decode('utf-8', 'replace').strip()}")
  stderr = proc.stderr.decode("utf-8", "replace").strip()
  return [line for line in proc.stdout.decode("utf-8", "replace").splitlines() if line], stderr


def nul_split(data: bytes) -> list[str]:
  return [os.fsdecode(part) for part in data.split(b"\0") if part]


def tracked_index(root: Path) -> tuple[list[str], dict[str, int], dict[str, str]]:
  """Mirrors release_files.py's `git ls-files -z --recurse-submodules`."""
  proc = _git(root, ["ls-files", "-z", "--recurse-submodules"])
  if proc.returncode != 0:
    raise GitError(f"git ls-files --recurse-submodules failed:\n{proc.stderr.decode('utf-8', 'replace').strip()}")
  tracked = nul_split(proc.stdout)

  modes: dict[str, int] = {}
  gitlinks: dict[str, str] = {}
  for record in nul_split(git_output(root, ["ls-files", "--stage", "-z"])):
    meta, tab, path = record.partition("\t")
    if not tab:
      continue
    parts = meta.split()
    if len(parts) == 3:
      mode = int(parts[0], 8)
      modes[path] = mode
      if mode == 0o160000:
        gitlinks[path] = parts[1]
  return tracked, modes, gitlinks


# ----------------------------------------------------------------------------
# Data model
# ----------------------------------------------------------------------------


@dataclasses.dataclass
class Entry:
  path: str
  verdict: str  # "included" | "excluded"
  reasons: list[str]
  archive_path: str | None = None
  kind: str = "file"  # file | symlink | symlink-dir | gitlink
  detail: str = ""


@dataclasses.dataclass
class DiskFile:
  kind: str  # file | symlink | symlink-dir
  target: str = ""


@dataclasses.dataclass
class SymlinkInfo:
  path: str
  target: str
  source: str  # index | disk
  broken: bool
  escapes_root: bool
  on_disk_state: str  # symlink | regular-file | missing


@dataclasses.dataclass
class SubmoduleInfo:
  path: str
  sha: str
  state: str  # clean | different-commit | conflict | not-initialized
  registered: bool
  initialized: bool
  used_by_zipapp: bool
  release_lists_contents: bool


@dataclasses.dataclass
class Report:
  root: str
  include_big_model: bool
  zip_included: list[Entry] = dataclasses.field(default_factory=list)
  zip_excluded: list[Entry] = dataclasses.field(default_factory=list)
  release_included: list[Entry] = dataclasses.field(default_factory=list)
  release_excluded: list[Entry] = dataclasses.field(default_factory=list)
  zipapp_only: list[dict] = dataclasses.field(default_factory=list)
  release_only: list[dict] = dataclasses.field(default_factory=list)
  untracked: list[dict] = dataclasses.field(default_factory=list)
  symlinks: list[SymlinkInfo] = dataclasses.field(default_factory=list)
  submodules: list[SubmoduleInfo] = dataclasses.field(default_factory=list)
  warnings: list[str] = dataclasses.field(default_factory=list)


# ----------------------------------------------------------------------------
# zipapp rule emulation (pack.py walk + copy())
# ----------------------------------------------------------------------------


def is_under(path: str, root_rel: str) -> bool:
  return path == root_rel or path.startswith(root_rel.rstrip("/") + "/")


def zip_archive_path(rel: str, dirs: dict[str, str]) -> str | None:
  for src, package in dirs.items():
    if is_under(rel, src):
      rest = rel[len(src) :].lstrip("/")
      return f"{package}/{rest}" if rest else package
  return None


def walk_zip_sources(root: Path, rules: ZipappRules) -> tuple[dict[str, DiskFile], list[str]]:
  """One walk per pack.py DIRS source; symlinks are never followed."""
  seen: dict[str, DiskFile] = {}
  warnings: list[str] = []
  for src in rules.dirs:
    srcdir = root / Path(src)
    if not srcdir.is_dir():  # follows links, like copytree; missing/uninitialized submodules land here
      warnings.append(
        f"zipapp source dir '{src}' is not a readable directory (submodule not checked out?); pack.py's shutil.copytree would fail and it contributes no files"
      )
      continue
    for dirpath, dirnames, filenames in os.walk(srcdir, followlinks=False):
      dirnames.sort()
      for name in sorted(dirnames):
        full = Path(dirpath) / name
        rel = full.relative_to(root).as_posix()
        if full.is_symlink():
          seen[rel] = DiskFile(kind="symlink-dir")
          dirnames.remove(name)  # pack.py would dereference it; we do not descend
      for name in sorted(filenames):
        full = Path(dirpath) / name
        rel = full.relative_to(root).as_posix()
        seen[rel] = DiskFile(kind="symlink" if full.is_symlink() else "file")
  return seen, warnings


def classify_zipapp(rel: str, rules: ZipappRules) -> tuple[str, list[str]]:
  """Reproduces pack.py copy(): endswith(ext) and not (exc in src)."""
  ext_hit = next((ext for ext in rules.exts if rel.endswith(ext)), None)
  exc_hit = [exc for exc in rules.exclude if exc in rel]
  reasons: list[str] = []
  if ext_hit is not None:
    reasons.append(f"suffix matches EXTS entry '{ext_hit}'")
  else:
    reasons.append(f"suffix matches no EXTS entry {list(rules.exts)}")
  if exc_hit:
    reasons.append(f"matches EXCLUDE substring {exc_hit!r}")
  included = ext_hit is not None and not exc_hit
  return ("included" if included else "excluded", reasons)


# ----------------------------------------------------------------------------
# release_files.py rule emulation
# ----------------------------------------------------------------------------


def classify_release(rel: str, rules: ReleaseRules, include_big_model: bool) -> tuple[str, list[str]]:
  # release_files.py checks the big-model prefix before the blacklist.
  if not include_big_model and rel.startswith(rules.big_model_prefix):
    return "excluded", [f"startswith big-model prefix '{rules.big_model_prefix}' (set {BIG_MODEL_ENV} to include)"]
  blacklisted = [p for p in rules.blacklist if re.search(p, rel)]
  whitelisted = [p for p in rules.whitelist if re.search(p, rel)]
  if blacklisted and not whitelisted:
    return "excluded", [f"matches blacklist pattern {p!r}" for p in blacklisted]
  if blacklisted and whitelisted:
    return "included", [f"matches blacklist {blacklisted!r} but whitelist {whitelisted!r} overrides it"]
  return "included", ["git-tracked and no blacklist pattern matches"]


# ----------------------------------------------------------------------------
# Submodules and symlinks
# ----------------------------------------------------------------------------


def discover_submodules(root: Path, gitlinks: dict[str, str], zip_dirs: dict[str, str]) -> list[SubmoduleInfo]:
  registered: set[str] = set()
  config_proc = _git(root, ["config", "-f", ".gitmodules", "--get-regexp", r"^submodule\..*\.path$"])
  if config_proc.returncode == 0:
    for line in config_proc.stdout.decode("utf-8", "replace").splitlines():
      _, _, value = line.partition(" ")
      if value.strip():
        registered.add(value.strip().replace("\\", "/"))

  infos: list[SubmoduleInfo] = []
  status_proc = _git(root, ["submodule", "status", "--recursive"])
  parsed: dict[str, tuple[str, str]] = {}
  if status_proc.returncode == 0:
    for line in status_proc.stdout.decode("utf-8", "replace").splitlines():
      match = re.match(r"^([ +\-U])([0-9a-fA-F]{40})\s+(\S.*?)(?:\s*\(.*\))?$", line)
      if match:
        flag, sha, path = match.groups()
        parsed[path.replace("\\", "/")] = (flag, sha)

  for path, sha in sorted(gitlinks.items()):
    flag, parsed_sha = parsed.get(path, ("?", ""))
    state = {" ": "clean", "+": "different-commit", "U": "conflict", "-": "not-initialized"}.get(flag, "unknown")
    initialized = flag not in ("-", "?") and (root / path / ".git").exists()
    if flag in (" ", "+", "U") and not initialized:
      initialized = True  # trust git when the work tree layout is unusual
    used = any(is_under(path, src) or is_under(src, path) for src in zip_dirs)
    infos.append(
      SubmoduleInfo(
        path=path,
        sha=parsed_sha or sha,
        state=state,
        registered=path in registered,
        initialized=initialized,
        used_by_zipapp=used,
        release_lists_contents=initialized,
      )
    )
  return infos


def index_symlink_target(root: Path, path: str) -> str:
  proc = _git(root, ["cat-file", "blob", f":{path}"])
  return proc.stdout.decode("utf-8", "replace").strip() if proc.returncode == 0 else ""


def symlink_on_disk_state(root: Path, path: str) -> tuple[str, str, bool, bool]:
  """Returns (state, target, broken, escapes_root)."""
  full = root / path
  if full.is_symlink():
    target = os.readlink(full).replace("\\", "/")
    resolved = (full.parent / os.readlink(full)).resolve(strict=False)
    broken = not resolved.exists()
    try:
      resolved.relative_to(root.resolve())
      escapes = False
    except ValueError:
      escapes = True
    return "symlink", target, broken, escapes
  if full.exists():
    return "regular-file", "", False, False
  return "missing", "", False, False


# ----------------------------------------------------------------------------
# Core comparison
# ----------------------------------------------------------------------------


def compare(
  root: os.PathLike | str = DEFAULT_ROOT, zip_rules: ZipappRules | None = None, release_rules: ReleaseRules | None = None, include_big_model: bool | None = None
) -> Report:
  root = Path(root).resolve()
  if include_big_model is None:
    include_big_model = bool(os.getenv(BIG_MODEL_ENV))
  zip_rules = zip_rules or load_zipapp_rules()
  release_rules = release_rules or load_release_rules(include_big_model=include_big_model)

  report = Report(root=str(root), include_big_model=include_big_model)

  # --- git side: tracked paths and index modes ---
  tracked, modes, gitlinks = tracked_index(root)
  tracked_set = set(tracked)

  # --- zipapp side: filesystem walk ---
  disk_files, walk_warnings = walk_zip_sources(root, zip_rules)
  report.warnings.extend(walk_warnings)

  zip_entries: dict[str, Entry] = {}
  for rel, disk in disk_files.items():
    if disk.kind == "symlink-dir":
      continue  # reported in the symlinks section; contents would need link dereferencing
    verdict, reasons = classify_zipapp(rel, zip_rules)
    kind = "symlink" if disk.kind == "symlink" else "file"
    entry = Entry(path=rel, verdict=verdict, reasons=reasons, archive_path=zip_archive_path(rel, zip_rules.dirs), kind=kind)
    zip_entries[rel] = entry
    (report.zip_included if verdict == "included" else report.zip_excluded).append(entry)

  # --- release_files side ---
  proc = _git(root, ["ls-files", "-z", "--recurse-submodules"])
  if proc.stderr.strip():
    report.warnings.append("git ls-files --recurse-submodules stderr: " + proc.stderr.decode("utf-8", "replace").strip())
  release_entries: dict[str, Entry] = {}
  for rel in tracked:
    verdict, reasons = classify_release(rel, release_rules, include_big_model)
    kind = "gitlink" if modes.get(rel) == 0o160000 else ("symlink" if modes.get(rel) == 0o120000 else "file")
    detail = ""
    if kind == "gitlink":
      detail = "gitlink path; release_files lists the bare path for an uninitialized submodule but none of its files"
    entry = Entry(path=rel, verdict=verdict, reasons=reasons, kind=kind, detail=detail)
    release_entries[rel] = entry
    (report.release_included if verdict == "included" else report.release_excluded).append(entry)

  # --- divergence on paths both sides evaluate ---
  for rel in sorted(set(zip_entries) & tracked_set):
    z = zip_entries[rel]
    r = release_entries[rel]
    if z.verdict == "included" and r.verdict == "excluded":
      report.zipapp_only.append({"path": rel, "zipapp": z.reasons, "release_files": r.reasons})
    elif z.verdict == "excluded" and r.verdict == "included":
      report.release_only.append({"path": rel, "zipapp": z.reasons, "release_files": r.reasons})

  # --- untracked / ignored working tree files (invisible to release_files) ---
  untracked_paths = set(nul_split(git_output(root, ["ls-files", "--others", "--exclude-standard", "-z"])))
  ignored_paths = set()
  for src in zip_rules.dirs:
    ignored_paths.update(nul_split(git_output(root, ["ls-files", "--others", "-i", "--exclude-standard", "-z", "--", src])))
  for rel in sorted(untracked_paths | ignored_paths):
    under_zip = zip_archive_path(rel, zip_rules.dirs) is not None
    item = {
      "path": rel,
      "ignored": rel in ignored_paths,
      "zipapp": None,
      "note": "outside pack.py DIRS; zipapp never sees it" if not under_zip else None,
    }
    if under_zip:
      verdict, reasons = classify_zipapp(rel, zip_rules)
      item["zipapp"] = {"verdict": verdict, "reasons": reasons}
      item["note"] = (
        "pack.py walks the working tree and would include this untracked file"
        if verdict == "included"
        else "pack.py walks the working tree but its rules exclude this untracked file"
      )
    report.untracked.append(item)

  # --- symlinks: git index (repo-wide) merged with on-disk findings under DIRS ---
  index_links = sorted(p for p, mode in modes.items() if mode == 0o120000)
  reported: set[str] = set()
  for rel in index_links:
    state, disk_target, broken, escapes = symlink_on_disk_state(root, rel)
    target = disk_target or index_symlink_target(root, rel)
    if state == "regular-file":
      broken = escapes = False
    info = SymlinkInfo(path=rel, target=target, source="index", broken=broken, escapes_root=escapes, on_disk_state=state)
    report.symlinks.append(info)
    reported.add(rel)
    if state == "regular-file":
      report.warnings.append(
        f"symlink '{rel}' is checked out as a regular file (core.symlinks=false); it is a symlink in git and pack.py would dereference it on Linux"
      )
  for rel, disk in sorted(disk_files.items()):
    if disk.kind not in ("symlink", "symlink-dir") or rel in reported:
      continue
    state, target, broken, escapes = symlink_on_disk_state(root, rel)
    info = SymlinkInfo(
      path=rel, target=target, source="disk", broken=broken, escapes_root=escapes, on_disk_state=state + ("-dir" if disk.kind == "symlink-dir" else "")
    )
    report.symlinks.append(info)
    reported.add(rel)
  for info in report.symlinks:
    if info.broken:
      report.warnings.append(
        f"broken symlink '{info.path}' -> '{info.target}': pack.py's copy2(follow_symlinks=True) would fail; release_files lists the link itself"
      )
    if "dir" in info.on_disk_state:
      report.warnings.append(
        f"symlinked directory '{info.path}' -> '{info.target}': pack.py dereferences it "
        f"(symlinks=False) and would copy the linked tree; contents are not enumerated here"
      )
  report.symlinks.sort(key=lambda s: s.path)

  # --- submodules ---
  report.submodules = discover_submodules(root, gitlinks, zip_rules.dirs)
  for sub in report.submodules:
    if not sub.registered:
      report.warnings.append(f"gitlink '{sub.path}' is not registered in .gitmodules")
    if not sub.initialized:
      bits = [f"submodule '{sub.path}' is not checked out"]
      if sub.used_by_zipapp:
        bits.append("pack.py needs it and its DIRS walk fails / contributes nothing")
      bits.append("git ls-files --recurse-submodules omits its contents silently (only the gitlink path is listed)")
      report.warnings.append("; ".join(bits))

  for entries in (report.zip_included, report.zip_excluded, report.release_included, report.release_excluded):
    entries.sort(key=lambda e: e.path)
  report.zipapp_only.sort(key=lambda d: d["path"])
  report.release_only.sort(key=lambda d: d["path"])
  report.submodules.sort(key=lambda s: s.path)
  return report


# ----------------------------------------------------------------------------
# Output
# ----------------------------------------------------------------------------


def _entry_line(entry: Entry) -> str:
  suffix = f"  [{entry.kind}]" if entry.kind != "file" else ""
  detail = f"  ({entry.detail})" if entry.detail else ""
  return f"  {entry.path}{suffix}  # {'; '.join(entry.reasons)}{detail}"


def render_text(report: Report) -> str:
  lines = [
    "openpilot release rule comparison (read-only)",
    f"root: {report.root}",
    f"{BIG_MODEL_ENV}: {'on' if report.include_big_model else 'off'}",
    "",
    "[summary]",
    f"  zipapp pack.py       : {len(report.zip_included)} included, {len(report.zip_excluded)} excluded",
    f"  release_files.py     : {len(report.release_included)} included, {len(report.release_excluded)} excluded",
    f"  divergences          : {len(report.zipapp_only)} zipapp-only, {len(report.release_only)} release-only",
    f"  not git-tracked files: {len(report.untracked)}",
    f"  symlinks             : {len(report.symlinks)}",
    f"  submodules           : {len(report.submodules)} ({sum(not s.initialized for s in report.submodules)} inaccessible)",
    f"  warnings             : {len(report.warnings)}",
    "",
    "== zipapp (pack.py: walk DIRS, keep EXTS suffixes, drop EXCLUDE substrings) ==",
    "included:",
  ]
  lines += [_entry_line(e) for e in report.zip_included] or ["  (none)"]
  lines.append("excluded:")
  lines += [_entry_line(e) for e in report.zip_excluded] or ["  (none)"]

  lines += [
    "",
    "== release_files.py (git ls-files --recurse-submodules, then blacklist/whitelist) ==",
    "included:",
  ]
  lines += [_entry_line(e) for e in report.release_included] or ["  (none)"]
  lines.append("excluded:")
  lines += [_entry_line(e) for e in report.release_excluded] or ["  (none)"]

  lines += ["", "== included by the zipapp but excluded from the release =="]
  lines += [f"  {d['path']}  # zipapp: {'; '.join(d['zipapp'])} | release_files: {'; '.join(d['release_files'])}" for d in report.zipapp_only] or ["  (none)"]
  lines.append("== included in the release but excluded from the zipapp ==")
  lines += [f"  {d['path']}  # zipapp: {'; '.join(d['zipapp'])} | release_files: {'; '.join(d['release_files'])}" for d in report.release_only] or ["  (none)"]

  lines += ["", "== working tree files not visible to release_files (untracked / ignored) =="]
  if report.untracked:
    for item in report.untracked:
      tag = "ignored" if item["ignored"] else "untracked"
      if item["zipapp"]:
        z = item["zipapp"]
        ztext = f"zipapp would be {z['verdict']} ({'; '.join(z['reasons'])})"
      else:
        ztext = "outside zipapp DIRS"
      lines.append(f"  {item['path']}  # git: {tag}; {ztext}")
  else:
    lines.append("  (none)")

  lines += ["", "== symlinks =="]
  if report.symlinks:
    for s in report.symlinks:
      flags = []
      if s.broken:
        flags.append("BROKEN")
      if s.escapes_root:
        flags.append("escapes repo root")
      flags.append(f"on disk: {s.on_disk_state}")
      lines.append(f"  {s.path}  # {s.source}; -> {s.target or '?'} [{', '.join(flags)}]")
  else:
    lines.append("  (none)")

  lines += ["", "== submodules =="]
  if report.submodules:
    for s in report.submodules:
      access = "accessible" if s.initialized else "INACCESSIBLE (not checked out)"
      uses = "used by pack.py DIRS" if s.used_by_zipapp else "not in pack.py DIRS"
      contents = "contents visible to release_files" if s.release_lists_contents else "contents hidden from release_files"
      lines.append(f"  {s.path}  # {s.state}, {access}, {uses}, {contents}, sha={s.sha[:12]}")
  else:
    lines.append("  (none)")

  lines += ["", "== warnings =="]
  lines += [f"  {w}" for w in report.warnings] or ["  (none)"]
  return "\n".join(lines) + "\n"


def report_to_dict(report: Report) -> dict:
  return dataclasses.asdict(report)


def main(argv: list[str] | None = None) -> int:
  parser = argparse.ArgumentParser(
    prog="compare_release_rules.py",
    description="Read-only comparison of pack.py zipapp rules and release_files.py rules",
  )
  parser.add_argument("--root", default=str(DEFAULT_ROOT), help="repo root (default: openpilot checkout)")
  parser.add_argument("--include-big-model", action="store_true", default=None,
                      help=f"emulate {BIG_MODEL_ENV}=1 (big driving model + .lfsconfig whitelist); "
                           f"without this flag the {BIG_MODEL_ENV} environment variable is honored")
  parser.add_argument("--json", action="store_true", help="emit machine-readable JSON")
  args = parser.parse_args(argv)

  root = Path(args.root)
  if not (root / ".git").exists() and not (root / ".git").is_file():
    print(f"not a git repository: {root}", file=sys.stderr)
    return 2
  try:
    report = compare(root=root, include_big_model=args.include_big_model)
  except (RuleSourceError, GitError) as e:
    print(str(e), file=sys.stderr)
    return 2

  if args.json:
    json.dump(report_to_dict(report), sys.stdout, indent=2, sort_keys=True)
    sys.stdout.write("\n")
  else:
    sys.stdout.write(render_text(report))
  return 0


if __name__ == "__main__":
  sys.exit(main())
