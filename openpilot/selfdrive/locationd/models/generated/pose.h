#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void pose_err_fun(double *nom_x, double *delta_x, double *out_207833252131504865);
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_5128581499076305372);
void pose_H_mod_fun(double *state, double *out_750146958085197188);
void pose_f_fun(double *state, double dt, double *out_6411414369012128450);
void pose_F_fun(double *state, double dt, double *out_503381519559238876);
void pose_h_4(double *state, double *unused, double *out_8930625875686900431);
void pose_H_4(double *state, double *unused, double *out_3021660251758710384);
void pose_h_10(double *state, double *unused, double *out_8232416234148166063);
void pose_H_10(double *state, double *unused, double *out_9213737062496983753);
void pose_h_13(double *state, double *unused, double *out_7562795773385764387);
void pose_H_13(double *state, double *unused, double *out_6233934077091043185);
void pose_h_14(double *state, double *unused, double *out_6527835941710948675);
void pose_H_14(double *state, double *unused, double *out_61128180536661912);
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt);
}