#include "car.h"

namespace {
#define DIM 9
#define EDIM 9
#define MEDIM 9
typedef void (*Hfun)(double *, double *, double *);

double rotational_inertia;

void set_rotational_inertia(double x){ rotational_inertia = x;}

double center_to_front;

void set_center_to_front(double x){ center_to_front = x;}

double center_to_rear;

void set_center_to_rear(double x){ center_to_rear = x;}

double stiffness_front;

void set_stiffness_front(double x){ stiffness_front = x;}

double stiffness_rear;

void set_stiffness_rear(double x){ stiffness_rear = x;}
const static double MAHA_THRESH_25 = 3.8414588206941227;
const static double MAHA_THRESH_24 = 5.991464547107981;
const static double MAHA_THRESH_30 = 3.8414588206941227;
const static double MAHA_THRESH_26 = 3.8414588206941227;
const static double MAHA_THRESH_27 = 3.8414588206941227;
const static double MAHA_THRESH_29 = 3.8414588206941227;
const static double MAHA_THRESH_28 = 3.8414588206941227;
const static double MAHA_THRESH_31 = 3.8414588206941227;

/******************************************************************************
 *                      Code generated with SymPy 1.14.0                      *
 *                                                                            *
 *              See http://www.sympy.org/ for more information.               *
 *                                                                            *
 *                         This file is part of 'ekf'                         *
 ******************************************************************************/
void err_fun(double *nom_x, double *delta_x, double *out_2630419627278583824) {
   out_2630419627278583824[0] = delta_x[0] + nom_x[0];
   out_2630419627278583824[1] = delta_x[1] + nom_x[1];
   out_2630419627278583824[2] = delta_x[2] + nom_x[2];
   out_2630419627278583824[3] = delta_x[3] + nom_x[3];
   out_2630419627278583824[4] = delta_x[4] + nom_x[4];
   out_2630419627278583824[5] = delta_x[5] + nom_x[5];
   out_2630419627278583824[6] = delta_x[6] + nom_x[6];
   out_2630419627278583824[7] = delta_x[7] + nom_x[7];
   out_2630419627278583824[8] = delta_x[8] + nom_x[8];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_8658744083896373911) {
   out_8658744083896373911[0] = -nom_x[0] + true_x[0];
   out_8658744083896373911[1] = -nom_x[1] + true_x[1];
   out_8658744083896373911[2] = -nom_x[2] + true_x[2];
   out_8658744083896373911[3] = -nom_x[3] + true_x[3];
   out_8658744083896373911[4] = -nom_x[4] + true_x[4];
   out_8658744083896373911[5] = -nom_x[5] + true_x[5];
   out_8658744083896373911[6] = -nom_x[6] + true_x[6];
   out_8658744083896373911[7] = -nom_x[7] + true_x[7];
   out_8658744083896373911[8] = -nom_x[8] + true_x[8];
}
void H_mod_fun(double *state, double *out_1469916003245605932) {
   out_1469916003245605932[0] = 1.0;
   out_1469916003245605932[1] = 0.0;
   out_1469916003245605932[2] = 0.0;
   out_1469916003245605932[3] = 0.0;
   out_1469916003245605932[4] = 0.0;
   out_1469916003245605932[5] = 0.0;
   out_1469916003245605932[6] = 0.0;
   out_1469916003245605932[7] = 0.0;
   out_1469916003245605932[8] = 0.0;
   out_1469916003245605932[9] = 0.0;
   out_1469916003245605932[10] = 1.0;
   out_1469916003245605932[11] = 0.0;
   out_1469916003245605932[12] = 0.0;
   out_1469916003245605932[13] = 0.0;
   out_1469916003245605932[14] = 0.0;
   out_1469916003245605932[15] = 0.0;
   out_1469916003245605932[16] = 0.0;
   out_1469916003245605932[17] = 0.0;
   out_1469916003245605932[18] = 0.0;
   out_1469916003245605932[19] = 0.0;
   out_1469916003245605932[20] = 1.0;
   out_1469916003245605932[21] = 0.0;
   out_1469916003245605932[22] = 0.0;
   out_1469916003245605932[23] = 0.0;
   out_1469916003245605932[24] = 0.0;
   out_1469916003245605932[25] = 0.0;
   out_1469916003245605932[26] = 0.0;
   out_1469916003245605932[27] = 0.0;
   out_1469916003245605932[28] = 0.0;
   out_1469916003245605932[29] = 0.0;
   out_1469916003245605932[30] = 1.0;
   out_1469916003245605932[31] = 0.0;
   out_1469916003245605932[32] = 0.0;
   out_1469916003245605932[33] = 0.0;
   out_1469916003245605932[34] = 0.0;
   out_1469916003245605932[35] = 0.0;
   out_1469916003245605932[36] = 0.0;
   out_1469916003245605932[37] = 0.0;
   out_1469916003245605932[38] = 0.0;
   out_1469916003245605932[39] = 0.0;
   out_1469916003245605932[40] = 1.0;
   out_1469916003245605932[41] = 0.0;
   out_1469916003245605932[42] = 0.0;
   out_1469916003245605932[43] = 0.0;
   out_1469916003245605932[44] = 0.0;
   out_1469916003245605932[45] = 0.0;
   out_1469916003245605932[46] = 0.0;
   out_1469916003245605932[47] = 0.0;
   out_1469916003245605932[48] = 0.0;
   out_1469916003245605932[49] = 0.0;
   out_1469916003245605932[50] = 1.0;
   out_1469916003245605932[51] = 0.0;
   out_1469916003245605932[52] = 0.0;
   out_1469916003245605932[53] = 0.0;
   out_1469916003245605932[54] = 0.0;
   out_1469916003245605932[55] = 0.0;
   out_1469916003245605932[56] = 0.0;
   out_1469916003245605932[57] = 0.0;
   out_1469916003245605932[58] = 0.0;
   out_1469916003245605932[59] = 0.0;
   out_1469916003245605932[60] = 1.0;
   out_1469916003245605932[61] = 0.0;
   out_1469916003245605932[62] = 0.0;
   out_1469916003245605932[63] = 0.0;
   out_1469916003245605932[64] = 0.0;
   out_1469916003245605932[65] = 0.0;
   out_1469916003245605932[66] = 0.0;
   out_1469916003245605932[67] = 0.0;
   out_1469916003245605932[68] = 0.0;
   out_1469916003245605932[69] = 0.0;
   out_1469916003245605932[70] = 1.0;
   out_1469916003245605932[71] = 0.0;
   out_1469916003245605932[72] = 0.0;
   out_1469916003245605932[73] = 0.0;
   out_1469916003245605932[74] = 0.0;
   out_1469916003245605932[75] = 0.0;
   out_1469916003245605932[76] = 0.0;
   out_1469916003245605932[77] = 0.0;
   out_1469916003245605932[78] = 0.0;
   out_1469916003245605932[79] = 0.0;
   out_1469916003245605932[80] = 1.0;
}
void f_fun(double *state, double dt, double *out_5998368294356086246) {
   out_5998368294356086246[0] = state[0];
   out_5998368294356086246[1] = state[1];
   out_5998368294356086246[2] = state[2];
   out_5998368294356086246[3] = state[3];
   out_5998368294356086246[4] = state[4];
   out_5998368294356086246[5] = dt*(stiffness_front*(-state[2] - state[3] + state[7])*state[0]/state[1] + (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/state[4] + ((-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/state[4] - state[4])*state[6] - 9.8100000000000005*state[8]) + state[5];
   out_5998368294356086246[6] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*state[4])) + state[6];
   out_5998368294356086246[7] = state[7];
   out_5998368294356086246[8] = state[8];
}
void F_fun(double *state, double dt, double *out_654274430748737504) {
   out_654274430748737504[0] = 1;
   out_654274430748737504[1] = 0;
   out_654274430748737504[2] = 0;
   out_654274430748737504[3] = 0;
   out_654274430748737504[4] = 0;
   out_654274430748737504[5] = 0;
   out_654274430748737504[6] = 0;
   out_654274430748737504[7] = 0;
   out_654274430748737504[8] = 0;
   out_654274430748737504[9] = 0;
   out_654274430748737504[10] = 1;
   out_654274430748737504[11] = 0;
   out_654274430748737504[12] = 0;
   out_654274430748737504[13] = 0;
   out_654274430748737504[14] = 0;
   out_654274430748737504[15] = 0;
   out_654274430748737504[16] = 0;
   out_654274430748737504[17] = 0;
   out_654274430748737504[18] = 0;
   out_654274430748737504[19] = 0;
   out_654274430748737504[20] = 1;
   out_654274430748737504[21] = 0;
   out_654274430748737504[22] = 0;
   out_654274430748737504[23] = 0;
   out_654274430748737504[24] = 0;
   out_654274430748737504[25] = 0;
   out_654274430748737504[26] = 0;
   out_654274430748737504[27] = 0;
   out_654274430748737504[28] = 0;
   out_654274430748737504[29] = 0;
   out_654274430748737504[30] = 1;
   out_654274430748737504[31] = 0;
   out_654274430748737504[32] = 0;
   out_654274430748737504[33] = 0;
   out_654274430748737504[34] = 0;
   out_654274430748737504[35] = 0;
   out_654274430748737504[36] = 0;
   out_654274430748737504[37] = 0;
   out_654274430748737504[38] = 0;
   out_654274430748737504[39] = 0;
   out_654274430748737504[40] = 1;
   out_654274430748737504[41] = 0;
   out_654274430748737504[42] = 0;
   out_654274430748737504[43] = 0;
   out_654274430748737504[44] = 0;
   out_654274430748737504[45] = dt*(stiffness_front*(-state[2] - state[3] + state[7])/state[1] + (-stiffness_front - stiffness_rear)*state[5]/state[4] + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[6]/state[4]);
   out_654274430748737504[46] = -dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/pow(state[1], 2);
   out_654274430748737504[47] = -dt*stiffness_front*state[0]/state[1];
   out_654274430748737504[48] = -dt*stiffness_front*state[0]/state[1];
   out_654274430748737504[49] = dt*(-(-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/pow(state[4], 2) + (-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/pow(state[4], 2) - 1)*state[6]);
   out_654274430748737504[50] = dt*(-stiffness_front*state[0] - stiffness_rear*state[0])/state[4] + 1;
   out_654274430748737504[51] = dt*((-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/state[4] - state[4]);
   out_654274430748737504[52] = dt*stiffness_front*state[0]/state[1];
   out_654274430748737504[53] = -9.8100000000000005*dt;
   out_654274430748737504[54] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front - pow(center_to_rear, 2)*stiffness_rear)*state[6]/(rotational_inertia*state[4]));
   out_654274430748737504[55] = -center_to_front*dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*pow(state[1], 2));
   out_654274430748737504[56] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_654274430748737504[57] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_654274430748737504[58] = dt*(-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*pow(state[4], 2)) - (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*pow(state[4], 2)));
   out_654274430748737504[59] = dt*(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(rotational_inertia*state[4]);
   out_654274430748737504[60] = dt*(-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])/(rotational_inertia*state[4]) + 1;
   out_654274430748737504[61] = center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_654274430748737504[62] = 0;
   out_654274430748737504[63] = 0;
   out_654274430748737504[64] = 0;
   out_654274430748737504[65] = 0;
   out_654274430748737504[66] = 0;
   out_654274430748737504[67] = 0;
   out_654274430748737504[68] = 0;
   out_654274430748737504[69] = 0;
   out_654274430748737504[70] = 1;
   out_654274430748737504[71] = 0;
   out_654274430748737504[72] = 0;
   out_654274430748737504[73] = 0;
   out_654274430748737504[74] = 0;
   out_654274430748737504[75] = 0;
   out_654274430748737504[76] = 0;
   out_654274430748737504[77] = 0;
   out_654274430748737504[78] = 0;
   out_654274430748737504[79] = 0;
   out_654274430748737504[80] = 1;
}
void h_25(double *state, double *unused, double *out_3707896474434790318) {
   out_3707896474434790318[0] = state[6];
}
void H_25(double *state, double *unused, double *out_3433775821939058908) {
   out_3433775821939058908[0] = 0;
   out_3433775821939058908[1] = 0;
   out_3433775821939058908[2] = 0;
   out_3433775821939058908[3] = 0;
   out_3433775821939058908[4] = 0;
   out_3433775821939058908[5] = 0;
   out_3433775821939058908[6] = 1;
   out_3433775821939058908[7] = 0;
   out_3433775821939058908[8] = 0;
}
void h_24(double *state, double *unused, double *out_2349893209276372167) {
   out_2349893209276372167[0] = state[4];
   out_2349893209276372167[1] = state[5];
}
void H_24(double *state, double *unused, double *out_5610990245546208881) {
   out_5610990245546208881[0] = 0;
   out_5610990245546208881[1] = 0;
   out_5610990245546208881[2] = 0;
   out_5610990245546208881[3] = 0;
   out_5610990245546208881[4] = 1;
   out_5610990245546208881[5] = 0;
   out_5610990245546208881[6] = 0;
   out_5610990245546208881[7] = 0;
   out_5610990245546208881[8] = 0;
   out_5610990245546208881[9] = 0;
   out_5610990245546208881[10] = 0;
   out_5610990245546208881[11] = 0;
   out_5610990245546208881[12] = 0;
   out_5610990245546208881[13] = 0;
   out_5610990245546208881[14] = 1;
   out_5610990245546208881[15] = 0;
   out_5610990245546208881[16] = 0;
   out_5610990245546208881[17] = 0;
}
void h_30(double *state, double *unused, double *out_5535702188662968199) {
   out_5535702188662968199[0] = state[4];
}
void H_30(double *state, double *unused, double *out_5952108780446307535) {
   out_5952108780446307535[0] = 0;
   out_5952108780446307535[1] = 0;
   out_5952108780446307535[2] = 0;
   out_5952108780446307535[3] = 0;
   out_5952108780446307535[4] = 1;
   out_5952108780446307535[5] = 0;
   out_5952108780446307535[6] = 0;
   out_5952108780446307535[7] = 0;
   out_5952108780446307535[8] = 0;
}
void h_26(double *state, double *unused, double *out_4193204965177891091) {
   out_4193204965177891091[0] = state[7];
}
void H_26(double *state, double *unused, double *out_6738301791699859509) {
   out_6738301791699859509[0] = 0;
   out_6738301791699859509[1] = 0;
   out_6738301791699859509[2] = 0;
   out_6738301791699859509[3] = 0;
   out_6738301791699859509[4] = 0;
   out_6738301791699859509[5] = 0;
   out_6738301791699859509[6] = 0;
   out_6738301791699859509[7] = 1;
   out_6738301791699859509[8] = 0;
}
void h_27(double *state, double *unused, double *out_573667366808870190) {
   out_573667366808870190[0] = state[3];
}
void H_27(double *state, double *unused, double *out_3777345468645882624) {
   out_3777345468645882624[0] = 0;
   out_3777345468645882624[1] = 0;
   out_3777345468645882624[2] = 0;
   out_3777345468645882624[3] = 1;
   out_3777345468645882624[4] = 0;
   out_3777345468645882624[5] = 0;
   out_3777345468645882624[6] = 0;
   out_3777345468645882624[7] = 0;
   out_3777345468645882624[8] = 0;
}
void h_29(double *state, double *unused, double *out_212909060831895971) {
   out_212909060831895971[0] = state[1];
}
void H_29(double *state, double *unused, double *out_6462340124760699719) {
   out_6462340124760699719[0] = 0;
   out_6462340124760699719[1] = 1;
   out_6462340124760699719[2] = 0;
   out_6462340124760699719[3] = 0;
   out_6462340124760699719[4] = 0;
   out_6462340124760699719[5] = 0;
   out_6462340124760699719[6] = 0;
   out_6462340124760699719[7] = 0;
   out_6462340124760699719[8] = 0;
}
void h_28(double *state, double *unused, double *out_5293296452446565947) {
   out_5293296452446565947[0] = state[0];
}
void H_28(double *state, double *unused, double *out_1379941107691169145) {
   out_1379941107691169145[0] = 1;
   out_1379941107691169145[1] = 0;
   out_1379941107691169145[2] = 0;
   out_1379941107691169145[3] = 0;
   out_1379941107691169145[4] = 0;
   out_1379941107691169145[5] = 0;
   out_1379941107691169145[6] = 0;
   out_1379941107691169145[7] = 0;
   out_1379941107691169145[8] = 0;
}
void h_31(double *state, double *unused, double *out_2187924622350956606) {
   out_2187924622350956606[0] = state[8];
}
void H_31(double *state, double *unused, double *out_6112093689466508033) {
   out_6112093689466508033[0] = 0;
   out_6112093689466508033[1] = 0;
   out_6112093689466508033[2] = 0;
   out_6112093689466508033[3] = 0;
   out_6112093689466508033[4] = 0;
   out_6112093689466508033[5] = 0;
   out_6112093689466508033[6] = 0;
   out_6112093689466508033[7] = 0;
   out_6112093689466508033[8] = 1;
}
#include <eigen3/Eigen/Dense>
#include <iostream>

typedef Eigen::Matrix<double, DIM, DIM, Eigen::RowMajor> DDM;
typedef Eigen::Matrix<double, EDIM, EDIM, Eigen::RowMajor> EEM;
typedef Eigen::Matrix<double, DIM, EDIM, Eigen::RowMajor> DEM;

void predict(double *in_x, double *in_P, double *in_Q, double dt) {
  typedef Eigen::Matrix<double, MEDIM, MEDIM, Eigen::RowMajor> RRM;

  double nx[DIM] = {0};
  double in_F[EDIM*EDIM] = {0};

  // functions from sympy
  f_fun(in_x, dt, nx);
  F_fun(in_x, dt, in_F);


  EEM F(in_F);
  EEM P(in_P);
  EEM Q(in_Q);

  RRM F_main = F.topLeftCorner(MEDIM, MEDIM);
  P.topLeftCorner(MEDIM, MEDIM) = (F_main * P.topLeftCorner(MEDIM, MEDIM)) * F_main.transpose();
  P.topRightCorner(MEDIM, EDIM - MEDIM) = F_main * P.topRightCorner(MEDIM, EDIM - MEDIM);
  P.bottomLeftCorner(EDIM - MEDIM, MEDIM) = P.bottomLeftCorner(EDIM - MEDIM, MEDIM) * F_main.transpose();

  P = P + dt*Q;

  // copy out state
  memcpy(in_x, nx, DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
}

// note: extra_args dim only correct when null space projecting
// otherwise 1
template <int ZDIM, int EADIM, bool MAHA_TEST>
void update(double *in_x, double *in_P, Hfun h_fun, Hfun H_fun, Hfun Hea_fun, double *in_z, double *in_R, double *in_ea, double MAHA_THRESHOLD) {
  typedef Eigen::Matrix<double, ZDIM, ZDIM, Eigen::RowMajor> ZZM;
  typedef Eigen::Matrix<double, ZDIM, DIM, Eigen::RowMajor> ZDM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, EDIM, Eigen::RowMajor> XEM;
  //typedef Eigen::Matrix<double, EDIM, ZDIM, Eigen::RowMajor> EZM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, 1> X1M;
  typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> XXM;

  double in_hx[ZDIM] = {0};
  double in_H[ZDIM * DIM] = {0};
  double in_H_mod[EDIM * DIM] = {0};
  double delta_x[EDIM] = {0};
  double x_new[DIM] = {0};


  // state x, P
  Eigen::Matrix<double, ZDIM, 1> z(in_z);
  EEM P(in_P);
  ZZM pre_R(in_R);

  // functions from sympy
  h_fun(in_x, in_ea, in_hx);
  H_fun(in_x, in_ea, in_H);
  ZDM pre_H(in_H);

  // get y (y = z - hx)
  Eigen::Matrix<double, ZDIM, 1> pre_y(in_hx); pre_y = z - pre_y;
  X1M y; XXM H; XXM R;
  if (Hea_fun){
    typedef Eigen::Matrix<double, ZDIM, EADIM, Eigen::RowMajor> ZAM;
    double in_Hea[ZDIM * EADIM] = {0};
    Hea_fun(in_x, in_ea, in_Hea);
    ZAM Hea(in_Hea);
    XXM A = Hea.transpose().fullPivLu().kernel();


    y = A.transpose() * pre_y;
    H = A.transpose() * pre_H;
    R = A.transpose() * pre_R * A;
  } else {
    y = pre_y;
    H = pre_H;
    R = pre_R;
  }
  // get modified H
  H_mod_fun(in_x, in_H_mod);
  DEM H_mod(in_H_mod);
  XEM H_err = H * H_mod;

  // Do mahalobis distance test
  if (MAHA_TEST){
    XXM a = (H_err * P * H_err.transpose() + R).inverse();
    double maha_dist = y.transpose() * a * y;
    if (maha_dist > MAHA_THRESHOLD){
      R = 1.0e16 * R;
    }
  }

  // Outlier resilient weighting
  double weight = 1;//(1.5)/(1 + y.squaredNorm()/R.sum());

  // kalman gains and I_KH
  XXM S = ((H_err * P) * H_err.transpose()) + R/weight;
  XEM KT = S.fullPivLu().solve(H_err * P.transpose());
  //EZM K = KT.transpose(); TODO: WHY DOES THIS NOT COMPILE?
  //EZM K = S.fullPivLu().solve(H_err * P.transpose()).transpose();
  //std::cout << "Here is the matrix rot:\n" << K << std::endl;
  EEM I_KH = Eigen::Matrix<double, EDIM, EDIM>::Identity() - (KT.transpose() * H_err);

  // update state by injecting dx
  Eigen::Matrix<double, EDIM, 1> dx(delta_x);
  dx  = (KT.transpose() * y);
  memcpy(delta_x, dx.data(), EDIM * sizeof(double));
  err_fun(in_x, delta_x, x_new);
  Eigen::Matrix<double, DIM, 1> x(x_new);

  // update cov
  P = ((I_KH * P) * I_KH.transpose()) + ((KT.transpose() * R) * KT);

  // copy out state
  memcpy(in_x, x.data(), DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
  memcpy(in_z, y.data(), y.rows() * sizeof(double));
}




}
extern "C" {

void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_25, H_25, NULL, in_z, in_R, in_ea, MAHA_THRESH_25);
}
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<2, 3, 0>(in_x, in_P, h_24, H_24, NULL, in_z, in_R, in_ea, MAHA_THRESH_24);
}
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_30, H_30, NULL, in_z, in_R, in_ea, MAHA_THRESH_30);
}
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_26, H_26, NULL, in_z, in_R, in_ea, MAHA_THRESH_26);
}
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_27, H_27, NULL, in_z, in_R, in_ea, MAHA_THRESH_27);
}
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_29, H_29, NULL, in_z, in_R, in_ea, MAHA_THRESH_29);
}
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_28, H_28, NULL, in_z, in_R, in_ea, MAHA_THRESH_28);
}
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_31, H_31, NULL, in_z, in_R, in_ea, MAHA_THRESH_31);
}
void car_err_fun(double *nom_x, double *delta_x, double *out_2630419627278583824) {
  err_fun(nom_x, delta_x, out_2630419627278583824);
}
void car_inv_err_fun(double *nom_x, double *true_x, double *out_8658744083896373911) {
  inv_err_fun(nom_x, true_x, out_8658744083896373911);
}
void car_H_mod_fun(double *state, double *out_1469916003245605932) {
  H_mod_fun(state, out_1469916003245605932);
}
void car_f_fun(double *state, double dt, double *out_5998368294356086246) {
  f_fun(state,  dt, out_5998368294356086246);
}
void car_F_fun(double *state, double dt, double *out_654274430748737504) {
  F_fun(state,  dt, out_654274430748737504);
}
void car_h_25(double *state, double *unused, double *out_3707896474434790318) {
  h_25(state, unused, out_3707896474434790318);
}
void car_H_25(double *state, double *unused, double *out_3433775821939058908) {
  H_25(state, unused, out_3433775821939058908);
}
void car_h_24(double *state, double *unused, double *out_2349893209276372167) {
  h_24(state, unused, out_2349893209276372167);
}
void car_H_24(double *state, double *unused, double *out_5610990245546208881) {
  H_24(state, unused, out_5610990245546208881);
}
void car_h_30(double *state, double *unused, double *out_5535702188662968199) {
  h_30(state, unused, out_5535702188662968199);
}
void car_H_30(double *state, double *unused, double *out_5952108780446307535) {
  H_30(state, unused, out_5952108780446307535);
}
void car_h_26(double *state, double *unused, double *out_4193204965177891091) {
  h_26(state, unused, out_4193204965177891091);
}
void car_H_26(double *state, double *unused, double *out_6738301791699859509) {
  H_26(state, unused, out_6738301791699859509);
}
void car_h_27(double *state, double *unused, double *out_573667366808870190) {
  h_27(state, unused, out_573667366808870190);
}
void car_H_27(double *state, double *unused, double *out_3777345468645882624) {
  H_27(state, unused, out_3777345468645882624);
}
void car_h_29(double *state, double *unused, double *out_212909060831895971) {
  h_29(state, unused, out_212909060831895971);
}
void car_H_29(double *state, double *unused, double *out_6462340124760699719) {
  H_29(state, unused, out_6462340124760699719);
}
void car_h_28(double *state, double *unused, double *out_5293296452446565947) {
  h_28(state, unused, out_5293296452446565947);
}
void car_H_28(double *state, double *unused, double *out_1379941107691169145) {
  H_28(state, unused, out_1379941107691169145);
}
void car_h_31(double *state, double *unused, double *out_2187924622350956606) {
  h_31(state, unused, out_2187924622350956606);
}
void car_H_31(double *state, double *unused, double *out_6112093689466508033) {
  H_31(state, unused, out_6112093689466508033);
}
void car_predict(double *in_x, double *in_P, double *in_Q, double dt) {
  predict(in_x, in_P, in_Q, dt);
}
void car_set_rotational_inertia(double x) {
  set_rotational_inertia(x);
}
void car_set_center_to_front(double x) {
  set_center_to_front(x);
}
void car_set_center_to_rear(double x) {
  set_center_to_rear(x);
}
void car_set_stiffness_front(double x) {
  set_stiffness_front(x);
}
void car_set_stiffness_rear(double x) {
  set_stiffness_rear(x);
}
}

const EKF car = {
  .name = "car",
  .kinds = { 25, 24, 30, 26, 27, 29, 28, 31 },
  .feature_kinds = {  },
  .f_fun = car_f_fun,
  .F_fun = car_F_fun,
  .err_fun = car_err_fun,
  .inv_err_fun = car_inv_err_fun,
  .H_mod_fun = car_H_mod_fun,
  .predict = car_predict,
  .hs = {
    { 25, car_h_25 },
    { 24, car_h_24 },
    { 30, car_h_30 },
    { 26, car_h_26 },
    { 27, car_h_27 },
    { 29, car_h_29 },
    { 28, car_h_28 },
    { 31, car_h_31 },
  },
  .Hs = {
    { 25, car_H_25 },
    { 24, car_H_24 },
    { 30, car_H_30 },
    { 26, car_H_26 },
    { 27, car_H_27 },
    { 29, car_H_29 },
    { 28, car_H_28 },
    { 31, car_H_31 },
  },
  .updates = {
    { 25, car_update_25 },
    { 24, car_update_24 },
    { 30, car_update_30 },
    { 26, car_update_26 },
    { 27, car_update_27 },
    { 29, car_update_29 },
    { 28, car_update_28 },
    { 31, car_update_31 },
  },
  .Hes = {
  },
  .sets = {
    { "rotational_inertia", car_set_rotational_inertia },
    { "center_to_front", car_set_center_to_front },
    { "center_to_rear", car_set_center_to_rear },
    { "stiffness_front", car_set_stiffness_front },
    { "stiffness_rear", car_set_stiffness_rear },
  },
  .extra_routines = {
  },
};

ekf_lib_init(car)
