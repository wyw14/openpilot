#pragma once
#include "rednose/helpers/ekf.h"
extern "C" {
void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_err_fun(double *nom_x, double *delta_x, double *out_2630419627278583824);
void car_inv_err_fun(double *nom_x, double *true_x, double *out_8658744083896373911);
void car_H_mod_fun(double *state, double *out_1469916003245605932);
void car_f_fun(double *state, double dt, double *out_5998368294356086246);
void car_F_fun(double *state, double dt, double *out_654274430748737504);
void car_h_25(double *state, double *unused, double *out_3707896474434790318);
void car_H_25(double *state, double *unused, double *out_3433775821939058908);
void car_h_24(double *state, double *unused, double *out_2349893209276372167);
void car_H_24(double *state, double *unused, double *out_5610990245546208881);
void car_h_30(double *state, double *unused, double *out_5535702188662968199);
void car_H_30(double *state, double *unused, double *out_5952108780446307535);
void car_h_26(double *state, double *unused, double *out_4193204965177891091);
void car_H_26(double *state, double *unused, double *out_6738301791699859509);
void car_h_27(double *state, double *unused, double *out_573667366808870190);
void car_H_27(double *state, double *unused, double *out_3777345468645882624);
void car_h_29(double *state, double *unused, double *out_212909060831895971);
void car_H_29(double *state, double *unused, double *out_6462340124760699719);
void car_h_28(double *state, double *unused, double *out_5293296452446565947);
void car_H_28(double *state, double *unused, double *out_1379941107691169145);
void car_h_31(double *state, double *unused, double *out_2187924622350956606);
void car_H_31(double *state, double *unused, double *out_6112093689466508033);
void car_predict(double *in_x, double *in_P, double *in_Q, double dt);
void car_set_rotational_inertia(double x);
void car_set_center_to_front(double x);
void car_set_center_to_rear(double x);
void car_set_stiffness_front(double x);
void car_set_stiffness_rear(double x);
}