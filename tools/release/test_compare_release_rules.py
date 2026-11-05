#!/usr/bin/env python3
import json
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest

HERE = Path(__file__).resolve().parent
REPO_ROOT = HERE.parents[1]
sys.path.insert(0, str(REPO_ROOT))

try:
  from tools.release.compare_release_rules import (
    BIG_MODEL_ENV,
    ZipappRules,
    classify_release,
    classify_zipapp,
    compare,
    load_release_rules,
    load_zipapp_rules,
    render_text,
    report_to_dict,
  )
except ModuleNotFoundError:
  sys.path.insert(0, str(HERE))
  from compare_release_rules import (  # type: ignore
    BIG_MODEL_ENV,
    ZipappRules,
    classify_release,
    classify_zipapp,
    compare,
    load_release_rules,
    load_zipapp_rules,
    render_text,
    report_to_dict,
  )


def git(cwd: Path, *args: str, check: bool = True, env: dict | None = None) -> subprocess.CompletedProcess:
  proc = subprocess.run(["git", "-C", str(cwd), *args], capture_output=True, env=env)
  if check and proc.returncode != 0:
    raise AssertionError(f"git {' '.join(args)} failed in {cwd}:\n{proc.stderr.decode('utf-8', 'replace')}")
  return proc


class TempRepo:
  def __init__(self, files: dict[str, str | bytes] | None = None):
    # pylint: disable=consider-using-with
    self.tmp = tempfile.TemporaryDirectory()
    self.path = Path(self.tmp.name)
    git(self.path, "init", "-q")
    git(self.path, "config", "user.email", "test@example.com")
    git(self.path, "config", "user.name", "test")
    for rel, content in (files or {}).items():
      self.write(rel, content)
    git(self.path, "add", "-A")
    git(self.path, "commit", "-q", "--allow-empty", "-m", "init")

  def write(self, rel: str, content: str | bytes = "") -> Path:
    full = self.path / rel
    full.parent.mkdir(parents=True, exist_ok=True)
    if isinstance(content, bytes):
      full.write_bytes(content)
    else:
      full.write_text(content, encoding="utf-8")
    return full

  def snapshot(self) -> dict[str, tuple[int, int]]:
    result = {}
    for path in self.path.rglob("*"):
      if ".git" in path.relative_to(self.path).parts:
        continue
      if path.is_file():
        stat = path.stat()
        result[path.relative_to(self.path).as_posix()] = (stat.st_size, stat.st_mtime_ns)
    return result

  def close(self) -> None:
    self.tmp.cleanup()


def by_path(entries: list) -> dict[str, object]:
  return {e.path: e for e in entries}


class TestRuleLoading(unittest.TestCase):
  def test_real_zipapp_rules(self):
    rules = load_zipapp_rules()
    self.assertEqual(rules.dirs["openpilot"], "openpilot")
    self.assertEqual(rules.dirs["opendbc_repo/opendbc"], "opendbc")
    self.assertIn(".py", rules.exts)
    self.assertIn(".png", rules.exts)
    self.assertNotIn(".pkl", rules.exts)
    self.assertIn("openpilot/selfdrive/assets/training", rules.exclude)

  def test_real_release_rules_and_big_model_switch(self):
    off = load_release_rules(include_big_model=False)
    on = load_release_rules(include_big_model=True)
    self.assertIn(".git/", off.blacklist)
    self.assertIn(".lfsconfig", off.blacklist)
    self.assertEqual(off.whitelist, ())
    self.assertEqual(on.whitelist, (r"^\.lfsconfig$",))
    self.assertEqual(off.big_model_prefix, "openpilot/selfdrive/modeld/models/big_")

  def test_classification_units(self):
    z = load_zipapp_rules()
    verdict, reasons = classify_zipapp("openpilot/a.py", z)
    self.assertEqual(verdict, "included")
    self.assertTrue(any(".py" in r for r in reasons))

    verdict, _ = classify_zipapp("openpilot/a.txt", z)
    self.assertEqual(verdict, "excluded")

    verdict, reasons = classify_zipapp("openpilot/selfdrive/assets/training/x.png", z)
    self.assertEqual(verdict, "excluded")
    self.assertTrue(any("EXCLUDE" in r for r in reasons))

    # suffix matching is case sensitive, just like endswith()
    verdict, _ = classify_zipapp("openpilot/IMG.PNG", z)
    self.assertEqual(verdict, "excluded")

    r = load_release_rules(include_big_model=False)
    big = "openpilot/selfdrive/modeld/models/big_driving.pkl"
    verdict, reasons = classify_release(big, r, False)
    self.assertEqual(verdict, "excluded")
    self.assertIn("big-model", reasons[0])
    verdict, _ = classify_release(big, r, True)
    self.assertEqual(verdict, "included")

    verdict, reasons = classify_release(".lfsconfig", r, False)
    self.assertEqual(verdict, "excluded")
    self.assertTrue(any("blacklist" in x for x in reasons))
    verdict, reasons = classify_release(".lfsconfig", load_release_rules(include_big_model=True), True)
    self.assertEqual(verdict, "included")
    self.assertTrue(any("whitelist" in x for x in reasons))


class TestCompareOnFixture(unittest.TestCase):
  def setUp(self):
    self.repo = TempRepo(
      {
        "openpilot/a.py": "a",
        "openpilot/b.txt": "b",
        "openpilot/IMG.PNG": "img",
        "openpilot/sub/c.png": "c",
        "openpilot/selfdrive/assets/training/step0.png": "t",
        "opendbc_repo/opendbc/dbc.py": "d",
        "notes.md": "n",
      }
    )
    self.addCleanup(self.repo.close)

  def test_zipapp_side(self):
    report = compare(self.repo.path)
    inc, exc = by_path(report.zip_included), by_path(report.zip_excluded)
    self.assertIn("openpilot/a.py", inc)
    self.assertIn("openpilot/sub/c.png", inc)
    self.assertIn("opendbc_repo/opendbc/dbc.py", inc)
    self.assertEqual(inc["opendbc_repo/opendbc/dbc.py"].archive_path, "opendbc/dbc.py")
    self.assertEqual(inc["openpilot/a.py"].archive_path, "openpilot/a.py")

    self.assertIn("openpilot/b.txt", exc)
    self.assertIn("openpilot/IMG.PNG", exc)  # case sensitive suffix
    training = exc["openpilot/selfdrive/assets/training/step0.png"]
    self.assertTrue(any("EXCLUDE" in r for r in training.reasons))

    self.assertNotIn("notes.md", inc)
    self.assertNotIn("notes.md", exc)  # outside DIRS, zipapp never walks it

    for entries in (report.zip_included, report.zip_excluded):
      paths = [e.path for e in entries]
      self.assertEqual(paths, sorted(paths))

  def test_release_side(self):
    self.repo.write(".lfsconfig", "x")
    self.repo.write(".gitattributes", "x")
    self.repo.write(".github/workflows/ci.yml", "x")
    self.repo.write("matlab.notes.md", "x")
    self.repo.write("openpilot/selfdrive/modeld/models/big_driving.pkl", "x")
    git(self.repo.path, "add", "-A")
    git(self.repo.path, "commit", "-q", "-m", "more")

    off = compare(self.repo.path, include_big_model=False)
    inc, exc = by_path(off.release_included), by_path(off.release_excluded)
    self.assertIn("openpilot/a.py", inc)
    self.assertIn("notes.md", inc)
    self.assertIn("openpilot/selfdrive/modeld/models/big_driving.pkl", exc)
    self.assertIn(".lfsconfig", exc)
    self.assertIn(".gitattributes", exc)
    self.assertIn(".github/workflows/ci.yml", exc)
    self.assertIn("matlab.notes.md", exc)

    on = compare(self.repo.path, include_big_model=True)
    inc_on = by_path(on.release_included)
    self.assertIn("openpilot/selfdrive/modeld/models/big_driving.pkl", inc_on)
    lfs = inc_on[".lfsconfig"]
    self.assertTrue(any("whitelist" in r for r in lfs.reasons))

  def test_untracked_and_ignored_files(self):
    self.repo.write(".gitignore", "openpilot/ignored.py\n")
    self.repo.write("openpilot/new.py", "x")  # untracked, zipapp would pack it
    self.repo.write("openpilot/new.txt", "x")  # untracked, fails EXTS
    self.repo.write("openpilot/ignored.py", "x")  # git-ignored
    self.repo.write("scratch.py", "x")  # untracked, outside DIRS
    report = compare(self.repo.path)

    items = {u["path"]: u for u in report.untracked}
    self.assertIn("openpilot/new.py", items)
    self.assertFalse(items["openpilot/new.py"]["ignored"])
    self.assertEqual(items["openpilot/new.py"]["zipapp"]["verdict"], "included")
    self.assertEqual(items["openpilot/new.txt"]["zipapp"]["verdict"], "excluded")
    self.assertTrue(items["openpilot/ignored.py"]["ignored"])
    self.assertIsNone(items["scratch.py"]["zipapp"])

    tracked_paths = {e.path for e in report.release_included + report.release_excluded}
    self.assertTrue({"openpilot/new.py", "openpilot/ignored.py", "scratch.py"}.isdisjoint(tracked_paths))

  def test_divergence_lists(self):
    report = compare(self.repo.path)
    # notes.md is tracked + outside zipapp roots, so it is not a divergence;
    # openpilot/b.txt is tracked but fails EXTS, so it is release-only.
    release_only = {d["path"] for d in report.release_only}
    self.assertIn("openpilot/b.txt", release_only)
    self.assertNotIn("notes.md", release_only)
    self.assertEqual(report.zipapp_only, [])


class TestEmptyResults(unittest.TestCase):
  def test_all_sections_empty(self):
    repo = TempRepo({".gitattributes": "x"})
    self.addCleanup(repo.close)
    rules = ZipappRules(dirs={"missing/dir": "x"}, exts=(".zzz",), exclude=())
    report = compare(repo.path, zip_rules=rules)
    self.assertEqual(report.zip_included, [])
    self.assertEqual(report.zip_excluded, [])
    self.assertEqual(report.release_included, [])
    self.assertEqual(report.untracked, [])
    self.assertEqual(report.symlinks, [])
    self.assertEqual(report.submodules, [])
    self.assertEqual([e.path for e in report.release_excluded], [".gitattributes"])
    self.assertTrue(any("missing/dir" in w for w in report.warnings))

    text = render_text(report)
    self.assertIn("(none)", text)
    parsed = json.loads(json.dumps(report_to_dict(report)))  # JSON serializable
    self.assertEqual(parsed["zip_included"], [])

  def test_empty_zipapp_include(self):
    repo = TempRepo({"openpilot/a.txt": "x"})
    self.addCleanup(repo.close)
    rules = ZipappRules(dirs={"openpilot": "openpilot"}, exts=(".zzz",), exclude=())
    report = compare(repo.path, zip_rules=rules)
    self.assertEqual(report.zip_included, [])
    self.assertEqual([e.path for e in report.zip_excluded], ["openpilot/a.txt"])
    self.assertEqual([e.path for e in report.release_included], ["openpilot/a.txt"])
    self.assertEqual(report.release_only[0]["path"], "openpilot/a.txt")


def can_create_symlinks() -> bool:
  try:
    with tempfile.TemporaryDirectory() as tmp:
      os.symlink("target", os.path.join(tmp, "link"))
    return True
  except (OSError, NotImplementedError):
    return False


@unittest.skipUnless(can_create_symlinks(), "symlink creation requires privileges on this platform")
class TestSymlinks(unittest.TestCase):
  def test_symlinks_reported(self):
    repo = TempRepo({"openpilot/real.py": "x"})
    self.addCleanup(repo.close)
    os.symlink("real.py", repo.path / "openpilot" / "link.py")
    os.symlink("does_not_exist.py", repo.path / "openpilot" / "broken.py")
    git(repo.path, "-c", "core.symlinks=true", "add", "-A")
    git(repo.path, "-c", "core.symlinks=true", "commit", "-q", "-m", "links")

    report = compare(repo.path)
    links = {s.path: s for s in report.symlinks}
    self.assertIn("openpilot/link.py", links)
    self.assertEqual(links["openpilot/link.py"].target.replace("\\", "/"), "real.py")
    self.assertFalse(links["openpilot/link.py"].broken)
    self.assertIn("openpilot/broken.py", links)
    self.assertTrue(links["openpilot/broken.py"].broken)
    self.assertTrue(any("broken symlink" in w for w in report.warnings))

    zip_inc = by_path(report.zip_included)
    self.assertEqual(zip_inc["openpilot/link.py"].kind, "symlink")


class TestSubmodules(unittest.TestCase):
  def _try_init(self):
    try:
      upstream = TempRepo({"opendbc/db.py": "x"})
    except Exception:
      self.skipTest("cannot build fixture repos")
    self.addCleanup(upstream.close)
    main = TempRepo()
    self.addCleanup(main.close)
    result = git(main.path, "-c", "protocol.file.allow=always", "submodule", "add", "--name", "opendbc", upstream.path.as_posix(), "opendbc_repo", check=False)
    if result.returncode != 0:
      self.skipTest("local submodule add unsupported:\n" + result.stderr.decode("utf-8", "replace"))
    git(main.path, "commit", "-q", "-m", "submodule")
    return upstream, main

  def test_initialized_and_inaccessible_submodule(self):
    _, main = self._try_init()
    rules = ZipappRules(dirs={"opendbc_repo/opendbc": "opendbc"}, exts=(".py",), exclude=())

    report = compare(main.path, zip_rules=rules)
    subs = {s.path: s for s in report.submodules}
    self.assertIn("opendbc_repo", subs)
    self.assertTrue(subs["opendbc_repo"].initialized)
    self.assertEqual(subs["opendbc_repo"].state, "clean")
    self.assertTrue(subs["opendbc_repo"].used_by_zipapp)
    tracked = {e.path for e in report.release_included}
    self.assertIn("opendbc_repo/opendbc/db.py", tracked)
    self.assertIn("opendbc_repo/opendbc/db.py", by_path(report.zip_included))

    git(main.path, "submodule", "deinit", "-f", "opendbc_repo")
    deinited = compare(main.path, zip_rules=rules)
    subs = {s.path: s for s in deinited.submodules}
    self.assertFalse(subs["opendbc_repo"].initialized)
    self.assertEqual(subs["opendbc_repo"].state, "not-initialized")
    self.assertFalse(subs["opendbc_repo"].release_lists_contents)
    self.assertTrue(any("opendbc_repo" in w and "not checked out" in w for w in deinited.warnings))
    self.assertTrue(any("opendbc_repo/opendbc" in w for w in deinited.warnings))  # zip walk root missing
    bare = by_path(deinited.release_included).get("opendbc_repo") or by_path(deinited.release_excluded).get("opendbc_repo")
    self.assertIsNotNone(bare)
    self.assertEqual(bare.kind, "gitlink")


class TestReadOnlyAndStability(unittest.TestCase):
  def test_no_worktree_modification_and_stable_order(self):
    repo = TempRepo({"openpilot/a.py": "x", "openpilot/dir/b.txt": "y"})
    self.addCleanup(repo.close)
    repo.write("openpilot/untracked.json", "{}")
    before = repo.snapshot()
    status_before = git(repo.path, "status", "--porcelain").stdout
    report1 = compare(repo.path)
    mid = repo.snapshot()
    report2 = compare(repo.path)
    after = repo.snapshot()
    self.assertEqual(before, mid)
    self.assertEqual(mid, after)
    self.assertEqual(git(repo.path, "status", "--porcelain").stdout, status_before)

    self.assertEqual(render_text(report1), render_text(report2))
    for entries in (report1.zip_included, report1.zip_excluded, report1.release_included, report1.release_excluded, report1.symlinks, report1.submodules):
      paths = [e.path for e in entries]
      self.assertEqual(paths, sorted(paths))


class TestRealRepoFaithfulness(unittest.TestCase):
  def test_matches_real_release_files_script(self):
    if not (REPO_ROOT / ".git").exists():
      self.skipTest("not running inside a git checkout")
    for include in (False, True):
      env = dict(os.environ)
      if include:
        env[BIG_MODEL_ENV] = "1"
      else:
        env.pop(BIG_MODEL_ENV, None)
      proc = subprocess.run([sys.executable, str(HERE / "release_files.py")], capture_output=True, cwd=REPO_ROOT, env=env)
      self.assertEqual(proc.returncode, 0, proc.stderr.decode("utf-8", "replace"))
      real = {os.fsdecode(p) for p in proc.stdout.split(b"\0") if p}
      report = compare(REPO_ROOT, include_big_model=include)
      mine = {e.path for e in report.release_included}
      self.assertEqual(real, mine, f"mismatch with INCLUDE_BIG_MODEL={include}")

  def test_real_tree_reports_uninitialized_submodules_and_symlinks(self):
    if not (REPO_ROOT / ".git").exists():
      self.skipTest("not running inside a git checkout")
    report = compare(REPO_ROOT)
    self.assertGreaterEqual(len(report.submodules), 1)
    self.assertTrue(all(not s.initialized for s in report.submodules), "fixture checkout has every submodule uninitialized")
    opendbc = next(s for s in report.submodules if s.path == "opendbc_repo")
    self.assertTrue(opendbc.used_by_zipapp)
    self.assertTrue(any("opendbc_repo/opendbc" in w for w in report.warnings))
    tracked_links = {s.path for s in report.symlinks if s.source == "index"}
    self.assertIn("openpilot/system/hardware/comma/agnos.json", tracked_links)


class TestCli(unittest.TestCase):
  def test_cli_json_and_bad_root(self):
    repo = TempRepo({"openpilot/a.py": "x"})
    self.addCleanup(repo.close)
    env = dict(os.environ)
    env.pop(BIG_MODEL_ENV, None)
    proc = subprocess.run([sys.executable, str(HERE / "compare_release_rules.py"), "--root", str(repo.path), "--json"], capture_output=True, env=env)
    self.assertEqual(proc.returncode, 0, proc.stderr.decode("utf-8", "replace"))
    data = json.loads(proc.stdout)
    self.assertFalse(data["include_big_model"])
    self.assertIn("openpilot/a.py", {e["path"] for e in data["zip_included"]})

    with tempfile.TemporaryDirectory() as empty:
      bad = subprocess.run([sys.executable, str(HERE / "compare_release_rules.py"), "--root", empty], capture_output=True, env=env)
      self.assertEqual(bad.returncode, 2)


if __name__ == "__main__":
  unittest.main()
