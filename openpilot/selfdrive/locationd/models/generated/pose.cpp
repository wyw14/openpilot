#include "pose.h"

namespace {
#define DIM 18
#define EDIM 18
#define MEDIM 18
typedef void (*Hfun)(double *, double *, double *);
const static double MAHA_THRESH_4 = 7.814727903251177;
const static double MAHA_THRESH_10 = 7.814727903251177;
const static double MAHA_THRESH_13 = 7.814727903251177;
const static double MAHA_THRESH_14 = 7.814727903251177;

/******************************************************************************
 *                      Code generated with SymPy 1.14.0                      *
 *                                                                            *
 *              See http://www.sympy.org/ for more information.               *
 *                                                                            *
 *                         This file is part of 'ekf'                         *
 ******************************************************************************/
void err_fun(double *nom_x, double *delta_x, double *out_207833252131504865) {
   out_207833252131504865[0] = delta_x[0] + nom_x[0];
   out_207833252131504865[1] = delta_x[1] + nom_x[1];
   out_207833252131504865[2] = delta_x[2] + nom_x[2];
   out_207833252131504865[3] = delta_x[3] + nom_x[3];
   out_207833252131504865[4] = delta_x[4] + nom_x[4];
   out_207833252131504865[5] = delta_x[5] + nom_x[5];
   out_207833252131504865[6] = delta_x[6] + nom_x[6];
   out_207833252131504865[7] = delta_x[7] + nom_x[7];
   out_207833252131504865[8] = delta_x[8] + nom_x[8];
   out_207833252131504865[9] = delta_x[9] + nom_x[9];
   out_207833252131504865[10] = delta_x[10] + nom_x[10];
   out_207833252131504865[11] = delta_x[11] + nom_x[11];
   out_207833252131504865[12] = delta_x[12] + nom_x[12];
   out_207833252131504865[13] = delta_x[13] + nom_x[13];
   out_207833252131504865[14] = delta_x[14] + nom_x[14];
   out_207833252131504865[15] = delta_x[15] + nom_x[15];
   out_207833252131504865[16] = delta_x[16] + nom_x[16];
   out_207833252131504865[17] = delta_x[17] + nom_x[17];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_5128581499076305372) {
   out_5128581499076305372[0] = -nom_x[0] + true_x[0];
   out_5128581499076305372[1] = -nom_x[1] + true_x[1];
   out_5128581499076305372[2] = -nom_x[2] + true_x[2];
   out_5128581499076305372[3] = -nom_x[3] + true_x[3];
   out_5128581499076305372[4] = -nom_x[4] + true_x[4];
   out_5128581499076305372[5] = -nom_x[5] + true_x[5];
   out_5128581499076305372[6] = -nom_x[6] + true_x[6];
   out_5128581499076305372[7] = -nom_x[7] + true_x[7];
   out_5128581499076305372[8] = -nom_x[8] + true_x[8];
   out_5128581499076305372[9] = -nom_x[9] + true_x[9];
   out_5128581499076305372[10] = -nom_x[10] + true_x[10];
   out_5128581499076305372[11] = -nom_x[11] + true_x[11];
   out_5128581499076305372[12] = -nom_x[12] + true_x[12];
   out_5128581499076305372[13] = -nom_x[13] + true_x[13];
   out_5128581499076305372[14] = -nom_x[14] + true_x[14];
   out_5128581499076305372[15] = -nom_x[15] + true_x[15];
   out_5128581499076305372[16] = -nom_x[16] + true_x[16];
   out_5128581499076305372[17] = -nom_x[17] + true_x[17];
}
void H_mod_fun(double *state, double *out_750146958085197188) {
   out_750146958085197188[0] = 1.0;
   out_750146958085197188[1] = 0.0;
   out_750146958085197188[2] = 0.0;
   out_750146958085197188[3] = 0.0;
   out_750146958085197188[4] = 0.0;
   out_750146958085197188[5] = 0.0;
   out_750146958085197188[6] = 0.0;
   out_750146958085197188[7] = 0.0;
   out_750146958085197188[8] = 0.0;
   out_750146958085197188[9] = 0.0;
   out_750146958085197188[10] = 0.0;
   out_750146958085197188[11] = 0.0;
   out_750146958085197188[12] = 0.0;
   out_750146958085197188[13] = 0.0;
   out_750146958085197188[14] = 0.0;
   out_750146958085197188[15] = 0.0;
   out_750146958085197188[16] = 0.0;
   out_750146958085197188[17] = 0.0;
   out_750146958085197188[18] = 0.0;
   out_750146958085197188[19] = 1.0;
   out_750146958085197188[20] = 0.0;
   out_750146958085197188[21] = 0.0;
   out_750146958085197188[22] = 0.0;
   out_750146958085197188[23] = 0.0;
   out_750146958085197188[24] = 0.0;
   out_750146958085197188[25] = 0.0;
   out_750146958085197188[26] = 0.0;
   out_750146958085197188[27] = 0.0;
   out_750146958085197188[28] = 0.0;
   out_750146958085197188[29] = 0.0;
   out_750146958085197188[30] = 0.0;
   out_750146958085197188[31] = 0.0;
   out_750146958085197188[32] = 0.0;
   out_750146958085197188[33] = 0.0;
   out_750146958085197188[34] = 0.0;
   out_750146958085197188[35] = 0.0;
   out_750146958085197188[36] = 0.0;
   out_750146958085197188[37] = 0.0;
   out_750146958085197188[38] = 1.0;
   out_750146958085197188[39] = 0.0;
   out_750146958085197188[40] = 0.0;
   out_750146958085197188[41] = 0.0;
   out_750146958085197188[42] = 0.0;
   out_750146958085197188[43] = 0.0;
   out_750146958085197188[44] = 0.0;
   out_750146958085197188[45] = 0.0;
   out_750146958085197188[46] = 0.0;
   out_750146958085197188[47] = 0.0;
   out_750146958085197188[48] = 0.0;
   out_750146958085197188[49] = 0.0;
   out_750146958085197188[50] = 0.0;
   out_750146958085197188[51] = 0.0;
   out_750146958085197188[52] = 0.0;
   out_750146958085197188[53] = 0.0;
   out_750146958085197188[54] = 0.0;
   out_750146958085197188[55] = 0.0;
   out_750146958085197188[56] = 0.0;
   out_750146958085197188[57] = 1.0;
   out_750146958085197188[58] = 0.0;
   out_750146958085197188[59] = 0.0;
   out_750146958085197188[60] = 0.0;
   out_750146958085197188[61] = 0.0;
   out_750146958085197188[62] = 0.0;
   out_750146958085197188[63] = 0.0;
   out_750146958085197188[64] = 0.0;
   out_750146958085197188[65] = 0.0;
   out_750146958085197188[66] = 0.0;
   out_750146958085197188[67] = 0.0;
   out_750146958085197188[68] = 0.0;
   out_750146958085197188[69] = 0.0;
   out_750146958085197188[70] = 0.0;
   out_750146958085197188[71] = 0.0;
   out_750146958085197188[72] = 0.0;
   out_750146958085197188[73] = 0.0;
   out_750146958085197188[74] = 0.0;
   out_750146958085197188[75] = 0.0;
   out_750146958085197188[76] = 1.0;
   out_750146958085197188[77] = 0.0;
   out_750146958085197188[78] = 0.0;
   out_750146958085197188[79] = 0.0;
   out_750146958085197188[80] = 0.0;
   out_750146958085197188[81] = 0.0;
   out_750146958085197188[82] = 0.0;
   out_750146958085197188[83] = 0.0;
   out_750146958085197188[84] = 0.0;
   out_750146958085197188[85] = 0.0;
   out_750146958085197188[86] = 0.0;
   out_750146958085197188[87] = 0.0;
   out_750146958085197188[88] = 0.0;
   out_750146958085197188[89] = 0.0;
   out_750146958085197188[90] = 0.0;
   out_750146958085197188[91] = 0.0;
   out_750146958085197188[92] = 0.0;
   out_750146958085197188[93] = 0.0;
   out_750146958085197188[94] = 0.0;
   out_750146958085197188[95] = 1.0;
   out_750146958085197188[96] = 0.0;
   out_750146958085197188[97] = 0.0;
   out_750146958085197188[98] = 0.0;
   out_750146958085197188[99] = 0.0;
   out_750146958085197188[100] = 0.0;
   out_750146958085197188[101] = 0.0;
   out_750146958085197188[102] = 0.0;
   out_750146958085197188[103] = 0.0;
   out_750146958085197188[104] = 0.0;
   out_750146958085197188[105] = 0.0;
   out_750146958085197188[106] = 0.0;
   out_750146958085197188[107] = 0.0;
   out_750146958085197188[108] = 0.0;
   out_750146958085197188[109] = 0.0;
   out_750146958085197188[110] = 0.0;
   out_750146958085197188[111] = 0.0;
   out_750146958085197188[112] = 0.0;
   out_750146958085197188[113] = 0.0;
   out_750146958085197188[114] = 1.0;
   out_750146958085197188[115] = 0.0;
   out_750146958085197188[116] = 0.0;
   out_750146958085197188[117] = 0.0;
   out_750146958085197188[118] = 0.0;
   out_750146958085197188[119] = 0.0;
   out_750146958085197188[120] = 0.0;
   out_750146958085197188[121] = 0.0;
   out_750146958085197188[122] = 0.0;
   out_750146958085197188[123] = 0.0;
   out_750146958085197188[124] = 0.0;
   out_750146958085197188[125] = 0.0;
   out_750146958085197188[126] = 0.0;
   out_750146958085197188[127] = 0.0;
   out_750146958085197188[128] = 0.0;
   out_750146958085197188[129] = 0.0;
   out_750146958085197188[130] = 0.0;
   out_750146958085197188[131] = 0.0;
   out_750146958085197188[132] = 0.0;
   out_750146958085197188[133] = 1.0;
   out_750146958085197188[134] = 0.0;
   out_750146958085197188[135] = 0.0;
   out_750146958085197188[136] = 0.0;
   out_750146958085197188[137] = 0.0;
   out_750146958085197188[138] = 0.0;
   out_750146958085197188[139] = 0.0;
   out_750146958085197188[140] = 0.0;
   out_750146958085197188[141] = 0.0;
   out_750146958085197188[142] = 0.0;
   out_750146958085197188[143] = 0.0;
   out_750146958085197188[144] = 0.0;
   out_750146958085197188[145] = 0.0;
   out_750146958085197188[146] = 0.0;
   out_750146958085197188[147] = 0.0;
   out_750146958085197188[148] = 0.0;
   out_750146958085197188[149] = 0.0;
   out_750146958085197188[150] = 0.0;
   out_750146958085197188[151] = 0.0;
   out_750146958085197188[152] = 1.0;
   out_750146958085197188[153] = 0.0;
   out_750146958085197188[154] = 0.0;
   out_750146958085197188[155] = 0.0;
   out_750146958085197188[156] = 0.0;
   out_750146958085197188[157] = 0.0;
   out_750146958085197188[158] = 0.0;
   out_750146958085197188[159] = 0.0;
   out_750146958085197188[160] = 0.0;
   out_750146958085197188[161] = 0.0;
   out_750146958085197188[162] = 0.0;
   out_750146958085197188[163] = 0.0;
   out_750146958085197188[164] = 0.0;
   out_750146958085197188[165] = 0.0;
   out_750146958085197188[166] = 0.0;
   out_750146958085197188[167] = 0.0;
   out_750146958085197188[168] = 0.0;
   out_750146958085197188[169] = 0.0;
   out_750146958085197188[170] = 0.0;
   out_750146958085197188[171] = 1.0;
   out_750146958085197188[172] = 0.0;
   out_750146958085197188[173] = 0.0;
   out_750146958085197188[174] = 0.0;
   out_750146958085197188[175] = 0.0;
   out_750146958085197188[176] = 0.0;
   out_750146958085197188[177] = 0.0;
   out_750146958085197188[178] = 0.0;
   out_750146958085197188[179] = 0.0;
   out_750146958085197188[180] = 0.0;
   out_750146958085197188[181] = 0.0;
   out_750146958085197188[182] = 0.0;
   out_750146958085197188[183] = 0.0;
   out_750146958085197188[184] = 0.0;
   out_750146958085197188[185] = 0.0;
   out_750146958085197188[186] = 0.0;
   out_750146958085197188[187] = 0.0;
   out_750146958085197188[188] = 0.0;
   out_750146958085197188[189] = 0.0;
   out_750146958085197188[190] = 1.0;
   out_750146958085197188[191] = 0.0;
   out_750146958085197188[192] = 0.0;
   out_750146958085197188[193] = 0.0;
   out_750146958085197188[194] = 0.0;
   out_750146958085197188[195] = 0.0;
   out_750146958085197188[196] = 0.0;
   out_750146958085197188[197] = 0.0;
   out_750146958085197188[198] = 0.0;
   out_750146958085197188[199] = 0.0;
   out_750146958085197188[200] = 0.0;
   out_750146958085197188[201] = 0.0;
   out_750146958085197188[202] = 0.0;
   out_750146958085197188[203] = 0.0;
   out_750146958085197188[204] = 0.0;
   out_750146958085197188[205] = 0.0;
   out_750146958085197188[206] = 0.0;
   out_750146958085197188[207] = 0.0;
   out_750146958085197188[208] = 0.0;
   out_750146958085197188[209] = 1.0;
   out_750146958085197188[210] = 0.0;
   out_750146958085197188[211] = 0.0;
   out_750146958085197188[212] = 0.0;
   out_750146958085197188[213] = 0.0;
   out_750146958085197188[214] = 0.0;
   out_750146958085197188[215] = 0.0;
   out_750146958085197188[216] = 0.0;
   out_750146958085197188[217] = 0.0;
   out_750146958085197188[218] = 0.0;
   out_750146958085197188[219] = 0.0;
   out_750146958085197188[220] = 0.0;
   out_750146958085197188[221] = 0.0;
   out_750146958085197188[222] = 0.0;
   out_750146958085197188[223] = 0.0;
   out_750146958085197188[224] = 0.0;
   out_750146958085197188[225] = 0.0;
   out_750146958085197188[226] = 0.0;
   out_750146958085197188[227] = 0.0;
   out_750146958085197188[228] = 1.0;
   out_750146958085197188[229] = 0.0;
   out_750146958085197188[230] = 0.0;
   out_750146958085197188[231] = 0.0;
   out_750146958085197188[232] = 0.0;
   out_750146958085197188[233] = 0.0;
   out_750146958085197188[234] = 0.0;
   out_750146958085197188[235] = 0.0;
   out_750146958085197188[236] = 0.0;
   out_750146958085197188[237] = 0.0;
   out_750146958085197188[238] = 0.0;
   out_750146958085197188[239] = 0.0;
   out_750146958085197188[240] = 0.0;
   out_750146958085197188[241] = 0.0;
   out_750146958085197188[242] = 0.0;
   out_750146958085197188[243] = 0.0;
   out_750146958085197188[244] = 0.0;
   out_750146958085197188[245] = 0.0;
   out_750146958085197188[246] = 0.0;
   out_750146958085197188[247] = 1.0;
   out_750146958085197188[248] = 0.0;
   out_750146958085197188[249] = 0.0;
   out_750146958085197188[250] = 0.0;
   out_750146958085197188[251] = 0.0;
   out_750146958085197188[252] = 0.0;
   out_750146958085197188[253] = 0.0;
   out_750146958085197188[254] = 0.0;
   out_750146958085197188[255] = 0.0;
   out_750146958085197188[256] = 0.0;
   out_750146958085197188[257] = 0.0;
   out_750146958085197188[258] = 0.0;
   out_750146958085197188[259] = 0.0;
   out_750146958085197188[260] = 0.0;
   out_750146958085197188[261] = 0.0;
   out_750146958085197188[262] = 0.0;
   out_750146958085197188[263] = 0.0;
   out_750146958085197188[264] = 0.0;
   out_750146958085197188[265] = 0.0;
   out_750146958085197188[266] = 1.0;
   out_750146958085197188[267] = 0.0;
   out_750146958085197188[268] = 0.0;
   out_750146958085197188[269] = 0.0;
   out_750146958085197188[270] = 0.0;
   out_750146958085197188[271] = 0.0;
   out_750146958085197188[272] = 0.0;
   out_750146958085197188[273] = 0.0;
   out_750146958085197188[274] = 0.0;
   out_750146958085197188[275] = 0.0;
   out_750146958085197188[276] = 0.0;
   out_750146958085197188[277] = 0.0;
   out_750146958085197188[278] = 0.0;
   out_750146958085197188[279] = 0.0;
   out_750146958085197188[280] = 0.0;
   out_750146958085197188[281] = 0.0;
   out_750146958085197188[282] = 0.0;
   out_750146958085197188[283] = 0.0;
   out_750146958085197188[284] = 0.0;
   out_750146958085197188[285] = 1.0;
   out_750146958085197188[286] = 0.0;
   out_750146958085197188[287] = 0.0;
   out_750146958085197188[288] = 0.0;
   out_750146958085197188[289] = 0.0;
   out_750146958085197188[290] = 0.0;
   out_750146958085197188[291] = 0.0;
   out_750146958085197188[292] = 0.0;
   out_750146958085197188[293] = 0.0;
   out_750146958085197188[294] = 0.0;
   out_750146958085197188[295] = 0.0;
   out_750146958085197188[296] = 0.0;
   out_750146958085197188[297] = 0.0;
   out_750146958085197188[298] = 0.0;
   out_750146958085197188[299] = 0.0;
   out_750146958085197188[300] = 0.0;
   out_750146958085197188[301] = 0.0;
   out_750146958085197188[302] = 0.0;
   out_750146958085197188[303] = 0.0;
   out_750146958085197188[304] = 1.0;
   out_750146958085197188[305] = 0.0;
   out_750146958085197188[306] = 0.0;
   out_750146958085197188[307] = 0.0;
   out_750146958085197188[308] = 0.0;
   out_750146958085197188[309] = 0.0;
   out_750146958085197188[310] = 0.0;
   out_750146958085197188[311] = 0.0;
   out_750146958085197188[312] = 0.0;
   out_750146958085197188[313] = 0.0;
   out_750146958085197188[314] = 0.0;
   out_750146958085197188[315] = 0.0;
   out_750146958085197188[316] = 0.0;
   out_750146958085197188[317] = 0.0;
   out_750146958085197188[318] = 0.0;
   out_750146958085197188[319] = 0.0;
   out_750146958085197188[320] = 0.0;
   out_750146958085197188[321] = 0.0;
   out_750146958085197188[322] = 0.0;
   out_750146958085197188[323] = 1.0;
}
void f_fun(double *state, double dt, double *out_6411414369012128450) {
   out_6411414369012128450[0] = atan2((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), -(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]));
   out_6411414369012128450[1] = asin(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]));
   out_6411414369012128450[2] = atan2(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), -(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]));
   out_6411414369012128450[3] = dt*state[12] + state[3];
   out_6411414369012128450[4] = dt*state[13] + state[4];
   out_6411414369012128450[5] = dt*state[14] + state[5];
   out_6411414369012128450[6] = state[6];
   out_6411414369012128450[7] = state[7];
   out_6411414369012128450[8] = state[8];
   out_6411414369012128450[9] = state[9];
   out_6411414369012128450[10] = state[10];
   out_6411414369012128450[11] = state[11];
   out_6411414369012128450[12] = state[12];
   out_6411414369012128450[13] = state[13];
   out_6411414369012128450[14] = state[14];
   out_6411414369012128450[15] = state[15];
   out_6411414369012128450[16] = state[16];
   out_6411414369012128450[17] = state[17];
}
void F_fun(double *state, double dt, double *out_503381519559238876) {
   out_503381519559238876[0] = ((-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*cos(state[0])*cos(state[1]) - sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*cos(state[0])*cos(state[1]) - sin(dt*state[6])*sin(state[0])*cos(dt*state[7])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_503381519559238876[1] = ((-sin(dt*state[6])*sin(dt*state[8]) - sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*cos(state[1]) - (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*sin(state[1]) - sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(state[0]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*sin(state[1]) + (-sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) + sin(dt*state[8])*cos(dt*state[6]))*cos(state[1]) - sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(state[0]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_503381519559238876[2] = 0;
   out_503381519559238876[3] = 0;
   out_503381519559238876[4] = 0;
   out_503381519559238876[5] = 0;
   out_503381519559238876[6] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(dt*cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) - dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_503381519559238876[7] = (-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[6])*sin(dt*state[7])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[6])*sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) - dt*sin(dt*state[6])*sin(state[1])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + (-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))*(-dt*sin(dt*state[7])*cos(dt*state[6])*cos(state[0])*cos(state[1]) + dt*sin(dt*state[8])*sin(state[0])*cos(dt*state[6])*cos(dt*state[7])*cos(state[1]) - dt*sin(state[1])*cos(dt*state[6])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_503381519559238876[8] = ((dt*sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + dt*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (dt*sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]))*(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2)) + ((dt*sin(dt*state[6])*sin(dt*state[8]) + dt*sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (-dt*sin(dt*state[6])*cos(dt*state[8]) + dt*sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]))*(-(sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) + (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) - sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/(pow(-(sin(dt*state[6])*sin(dt*state[8]) + sin(dt*state[7])*cos(dt*state[6])*cos(dt*state[8]))*sin(state[1]) + (-sin(dt*state[6])*cos(dt*state[8]) + sin(dt*state[7])*sin(dt*state[8])*cos(dt*state[6]))*sin(state[0])*cos(state[1]) + cos(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2) + pow((sin(dt*state[6])*sin(dt*state[7])*sin(dt*state[8]) + cos(dt*state[6])*cos(dt*state[8]))*sin(state[0])*cos(state[1]) - (sin(dt*state[6])*sin(dt*state[7])*cos(dt*state[8]) - sin(dt*state[8])*cos(dt*state[6]))*sin(state[1]) + sin(dt*state[6])*cos(dt*state[7])*cos(state[0])*cos(state[1]), 2));
   out_503381519559238876[9] = 0;
   out_503381519559238876[10] = 0;
   out_503381519559238876[11] = 0;
   out_503381519559238876[12] = 0;
   out_503381519559238876[13] = 0;
   out_503381519559238876[14] = 0;
   out_503381519559238876[15] = 0;
   out_503381519559238876[16] = 0;
   out_503381519559238876[17] = 0;
   out_503381519559238876[18] = (-sin(dt*state[7])*sin(state[0])*cos(state[1]) - sin(dt*state[8])*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_503381519559238876[19] = (-sin(dt*state[7])*sin(state[1])*cos(state[0]) + sin(dt*state[8])*sin(state[0])*sin(state[1])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_503381519559238876[20] = 0;
   out_503381519559238876[21] = 0;
   out_503381519559238876[22] = 0;
   out_503381519559238876[23] = 0;
   out_503381519559238876[24] = 0;
   out_503381519559238876[25] = (dt*sin(dt*state[7])*sin(dt*state[8])*sin(state[0])*cos(state[1]) - dt*sin(dt*state[7])*sin(state[1])*cos(dt*state[8]) + dt*cos(dt*state[7])*cos(state[0])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_503381519559238876[26] = (-dt*sin(dt*state[8])*sin(state[1])*cos(dt*state[7]) - dt*sin(state[0])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/sqrt(1 - pow(sin(dt*state[7])*cos(state[0])*cos(state[1]) - sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1]) + sin(state[1])*cos(dt*state[7])*cos(dt*state[8]), 2));
   out_503381519559238876[27] = 0;
   out_503381519559238876[28] = 0;
   out_503381519559238876[29] = 0;
   out_503381519559238876[30] = 0;
   out_503381519559238876[31] = 0;
   out_503381519559238876[32] = 0;
   out_503381519559238876[33] = 0;
   out_503381519559238876[34] = 0;
   out_503381519559238876[35] = 0;
   out_503381519559238876[36] = ((sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_503381519559238876[37] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-sin(dt*state[7])*sin(state[2])*cos(state[0])*cos(state[1]) + sin(dt*state[8])*sin(state[0])*sin(state[2])*cos(dt*state[7])*cos(state[1]) - sin(state[1])*sin(state[2])*cos(dt*state[7])*cos(dt*state[8]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(-sin(dt*state[7])*cos(state[0])*cos(state[1])*cos(state[2]) + sin(dt*state[8])*sin(state[0])*cos(dt*state[7])*cos(state[1])*cos(state[2]) - sin(state[1])*cos(dt*state[7])*cos(dt*state[8])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_503381519559238876[38] = ((-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (-sin(state[0])*sin(state[1])*sin(state[2]) - cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_503381519559238876[39] = 0;
   out_503381519559238876[40] = 0;
   out_503381519559238876[41] = 0;
   out_503381519559238876[42] = 0;
   out_503381519559238876[43] = (-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))*(dt*(sin(state[0])*cos(state[2]) - sin(state[1])*sin(state[2])*cos(state[0]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*sin(state[2])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + ((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))*(dt*(-sin(state[0])*sin(state[2]) - sin(state[1])*cos(state[0])*cos(state[2]))*cos(dt*state[7]) - dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[7])*sin(dt*state[8]) - dt*sin(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_503381519559238876[44] = (dt*(sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*sin(state[2])*cos(dt*state[7])*cos(state[1]))*(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2)) + (dt*(sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*cos(dt*state[7])*cos(dt*state[8]) - dt*sin(dt*state[8])*cos(dt*state[7])*cos(state[1])*cos(state[2]))*((-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) - (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) - sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]))/(pow(-(sin(state[0])*sin(state[2]) + sin(state[1])*cos(state[0])*cos(state[2]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*cos(state[2]) - sin(state[2])*cos(state[0]))*sin(dt*state[8])*cos(dt*state[7]) + cos(dt*state[7])*cos(dt*state[8])*cos(state[1])*cos(state[2]), 2) + pow(-(-sin(state[0])*cos(state[2]) + sin(state[1])*sin(state[2])*cos(state[0]))*sin(dt*state[7]) + (sin(state[0])*sin(state[1])*sin(state[2]) + cos(state[0])*cos(state[2]))*sin(dt*state[8])*cos(dt*state[7]) + sin(state[2])*cos(dt*state[7])*cos(dt*state[8])*cos(state[1]), 2));
   out_503381519559238876[45] = 0;
   out_503381519559238876[46] = 0;
   out_503381519559238876[47] = 0;
   out_503381519559238876[48] = 0;
   out_503381519559238876[49] = 0;
   out_503381519559238876[50] = 0;
   out_503381519559238876[51] = 0;
   out_503381519559238876[52] = 0;
   out_503381519559238876[53] = 0;
   out_503381519559238876[54] = 0;
   out_503381519559238876[55] = 0;
   out_503381519559238876[56] = 0;
   out_503381519559238876[57] = 1;
   out_503381519559238876[58] = 0;
   out_503381519559238876[59] = 0;
   out_503381519559238876[60] = 0;
   out_503381519559238876[61] = 0;
   out_503381519559238876[62] = 0;
   out_503381519559238876[63] = 0;
   out_503381519559238876[64] = 0;
   out_503381519559238876[65] = 0;
   out_503381519559238876[66] = dt;
   out_503381519559238876[67] = 0;
   out_503381519559238876[68] = 0;
   out_503381519559238876[69] = 0;
   out_503381519559238876[70] = 0;
   out_503381519559238876[71] = 0;
   out_503381519559238876[72] = 0;
   out_503381519559238876[73] = 0;
   out_503381519559238876[74] = 0;
   out_503381519559238876[75] = 0;
   out_503381519559238876[76] = 1;
   out_503381519559238876[77] = 0;
   out_503381519559238876[78] = 0;
   out_503381519559238876[79] = 0;
   out_503381519559238876[80] = 0;
   out_503381519559238876[81] = 0;
   out_503381519559238876[82] = 0;
   out_503381519559238876[83] = 0;
   out_503381519559238876[84] = 0;
   out_503381519559238876[85] = dt;
   out_503381519559238876[86] = 0;
   out_503381519559238876[87] = 0;
   out_503381519559238876[88] = 0;
   out_503381519559238876[89] = 0;
   out_503381519559238876[90] = 0;
   out_503381519559238876[91] = 0;
   out_503381519559238876[92] = 0;
   out_503381519559238876[93] = 0;
   out_503381519559238876[94] = 0;
   out_503381519559238876[95] = 1;
   out_503381519559238876[96] = 0;
   out_503381519559238876[97] = 0;
   out_503381519559238876[98] = 0;
   out_503381519559238876[99] = 0;
   out_503381519559238876[100] = 0;
   out_503381519559238876[101] = 0;
   out_503381519559238876[102] = 0;
   out_503381519559238876[103] = 0;
   out_503381519559238876[104] = dt;
   out_503381519559238876[105] = 0;
   out_503381519559238876[106] = 0;
   out_503381519559238876[107] = 0;
   out_503381519559238876[108] = 0;
   out_503381519559238876[109] = 0;
   out_503381519559238876[110] = 0;
   out_503381519559238876[111] = 0;
   out_503381519559238876[112] = 0;
   out_503381519559238876[113] = 0;
   out_503381519559238876[114] = 1;
   out_503381519559238876[115] = 0;
   out_503381519559238876[116] = 0;
   out_503381519559238876[117] = 0;
   out_503381519559238876[118] = 0;
   out_503381519559238876[119] = 0;
   out_503381519559238876[120] = 0;
   out_503381519559238876[121] = 0;
   out_503381519559238876[122] = 0;
   out_503381519559238876[123] = 0;
   out_503381519559238876[124] = 0;
   out_503381519559238876[125] = 0;
   out_503381519559238876[126] = 0;
   out_503381519559238876[127] = 0;
   out_503381519559238876[128] = 0;
   out_503381519559238876[129] = 0;
   out_503381519559238876[130] = 0;
   out_503381519559238876[131] = 0;
   out_503381519559238876[132] = 0;
   out_503381519559238876[133] = 1;
   out_503381519559238876[134] = 0;
   out_503381519559238876[135] = 0;
   out_503381519559238876[136] = 0;
   out_503381519559238876[137] = 0;
   out_503381519559238876[138] = 0;
   out_503381519559238876[139] = 0;
   out_503381519559238876[140] = 0;
   out_503381519559238876[141] = 0;
   out_503381519559238876[142] = 0;
   out_503381519559238876[143] = 0;
   out_503381519559238876[144] = 0;
   out_503381519559238876[145] = 0;
   out_503381519559238876[146] = 0;
   out_503381519559238876[147] = 0;
   out_503381519559238876[148] = 0;
   out_503381519559238876[149] = 0;
   out_503381519559238876[150] = 0;
   out_503381519559238876[151] = 0;
   out_503381519559238876[152] = 1;
   out_503381519559238876[153] = 0;
   out_503381519559238876[154] = 0;
   out_503381519559238876[155] = 0;
   out_503381519559238876[156] = 0;
   out_503381519559238876[157] = 0;
   out_503381519559238876[158] = 0;
   out_503381519559238876[159] = 0;
   out_503381519559238876[160] = 0;
   out_503381519559238876[161] = 0;
   out_503381519559238876[162] = 0;
   out_503381519559238876[163] = 0;
   out_503381519559238876[164] = 0;
   out_503381519559238876[165] = 0;
   out_503381519559238876[166] = 0;
   out_503381519559238876[167] = 0;
   out_503381519559238876[168] = 0;
   out_503381519559238876[169] = 0;
   out_503381519559238876[170] = 0;
   out_503381519559238876[171] = 1;
   out_503381519559238876[172] = 0;
   out_503381519559238876[173] = 0;
   out_503381519559238876[174] = 0;
   out_503381519559238876[175] = 0;
   out_503381519559238876[176] = 0;
   out_503381519559238876[177] = 0;
   out_503381519559238876[178] = 0;
   out_503381519559238876[179] = 0;
   out_503381519559238876[180] = 0;
   out_503381519559238876[181] = 0;
   out_503381519559238876[182] = 0;
   out_503381519559238876[183] = 0;
   out_503381519559238876[184] = 0;
   out_503381519559238876[185] = 0;
   out_503381519559238876[186] = 0;
   out_503381519559238876[187] = 0;
   out_503381519559238876[188] = 0;
   out_503381519559238876[189] = 0;
   out_503381519559238876[190] = 1;
   out_503381519559238876[191] = 0;
   out_503381519559238876[192] = 0;
   out_503381519559238876[193] = 0;
   out_503381519559238876[194] = 0;
   out_503381519559238876[195] = 0;
   out_503381519559238876[196] = 0;
   out_503381519559238876[197] = 0;
   out_503381519559238876[198] = 0;
   out_503381519559238876[199] = 0;
   out_503381519559238876[200] = 0;
   out_503381519559238876[201] = 0;
   out_503381519559238876[202] = 0;
   out_503381519559238876[203] = 0;
   out_503381519559238876[204] = 0;
   out_503381519559238876[205] = 0;
   out_503381519559238876[206] = 0;
   out_503381519559238876[207] = 0;
   out_503381519559238876[208] = 0;
   out_503381519559238876[209] = 1;
   out_503381519559238876[210] = 0;
   out_503381519559238876[211] = 0;
   out_503381519559238876[212] = 0;
   out_503381519559238876[213] = 0;
   out_503381519559238876[214] = 0;
   out_503381519559238876[215] = 0;
   out_503381519559238876[216] = 0;
   out_503381519559238876[217] = 0;
   out_503381519559238876[218] = 0;
   out_503381519559238876[219] = 0;
   out_503381519559238876[220] = 0;
   out_503381519559238876[221] = 0;
   out_503381519559238876[222] = 0;
   out_503381519559238876[223] = 0;
   out_503381519559238876[224] = 0;
   out_503381519559238876[225] = 0;
   out_503381519559238876[226] = 0;
   out_503381519559238876[227] = 0;
   out_503381519559238876[228] = 1;
   out_503381519559238876[229] = 0;
   out_503381519559238876[230] = 0;
   out_503381519559238876[231] = 0;
   out_503381519559238876[232] = 0;
   out_503381519559238876[233] = 0;
   out_503381519559238876[234] = 0;
   out_503381519559238876[235] = 0;
   out_503381519559238876[236] = 0;
   out_503381519559238876[237] = 0;
   out_503381519559238876[238] = 0;
   out_503381519559238876[239] = 0;
   out_503381519559238876[240] = 0;
   out_503381519559238876[241] = 0;
   out_503381519559238876[242] = 0;
   out_503381519559238876[243] = 0;
   out_503381519559238876[244] = 0;
   out_503381519559238876[245] = 0;
   out_503381519559238876[246] = 0;
   out_503381519559238876[247] = 1;
   out_503381519559238876[248] = 0;
   out_503381519559238876[249] = 0;
   out_503381519559238876[250] = 0;
   out_503381519559238876[251] = 0;
   out_503381519559238876[252] = 0;
   out_503381519559238876[253] = 0;
   out_503381519559238876[254] = 0;
   out_503381519559238876[255] = 0;
   out_503381519559238876[256] = 0;
   out_503381519559238876[257] = 0;
   out_503381519559238876[258] = 0;
   out_503381519559238876[259] = 0;
   out_503381519559238876[260] = 0;
   out_503381519559238876[261] = 0;
   out_503381519559238876[262] = 0;
   out_503381519559238876[263] = 0;
   out_503381519559238876[264] = 0;
   out_503381519559238876[265] = 0;
   out_503381519559238876[266] = 1;
   out_503381519559238876[267] = 0;
   out_503381519559238876[268] = 0;
   out_503381519559238876[269] = 0;
   out_503381519559238876[270] = 0;
   out_503381519559238876[271] = 0;
   out_503381519559238876[272] = 0;
   out_503381519559238876[273] = 0;
   out_503381519559238876[274] = 0;
   out_503381519559238876[275] = 0;
   out_503381519559238876[276] = 0;
   out_503381519559238876[277] = 0;
   out_503381519559238876[278] = 0;
   out_503381519559238876[279] = 0;
   out_503381519559238876[280] = 0;
   out_503381519559238876[281] = 0;
   out_503381519559238876[282] = 0;
   out_503381519559238876[283] = 0;
   out_503381519559238876[284] = 0;
   out_503381519559238876[285] = 1;
   out_503381519559238876[286] = 0;
   out_503381519559238876[287] = 0;
   out_503381519559238876[288] = 0;
   out_503381519559238876[289] = 0;
   out_503381519559238876[290] = 0;
   out_503381519559238876[291] = 0;
   out_503381519559238876[292] = 0;
   out_503381519559238876[293] = 0;
   out_503381519559238876[294] = 0;
   out_503381519559238876[295] = 0;
   out_503381519559238876[296] = 0;
   out_503381519559238876[297] = 0;
   out_503381519559238876[298] = 0;
   out_503381519559238876[299] = 0;
   out_503381519559238876[300] = 0;
   out_503381519559238876[301] = 0;
   out_503381519559238876[302] = 0;
   out_503381519559238876[303] = 0;
   out_503381519559238876[304] = 1;
   out_503381519559238876[305] = 0;
   out_503381519559238876[306] = 0;
   out_503381519559238876[307] = 0;
   out_503381519559238876[308] = 0;
   out_503381519559238876[309] = 0;
   out_503381519559238876[310] = 0;
   out_503381519559238876[311] = 0;
   out_503381519559238876[312] = 0;
   out_503381519559238876[313] = 0;
   out_503381519559238876[314] = 0;
   out_503381519559238876[315] = 0;
   out_503381519559238876[316] = 0;
   out_503381519559238876[317] = 0;
   out_503381519559238876[318] = 0;
   out_503381519559238876[319] = 0;
   out_503381519559238876[320] = 0;
   out_503381519559238876[321] = 0;
   out_503381519559238876[322] = 0;
   out_503381519559238876[323] = 1;
}
void h_4(double *state, double *unused, double *out_8930625875686900431) {
   out_8930625875686900431[0] = state[6] + state[9];
   out_8930625875686900431[1] = state[7] + state[10];
   out_8930625875686900431[2] = state[8] + state[11];
}
void H_4(double *state, double *unused, double *out_3021660251758710384) {
   out_3021660251758710384[0] = 0;
   out_3021660251758710384[1] = 0;
   out_3021660251758710384[2] = 0;
   out_3021660251758710384[3] = 0;
   out_3021660251758710384[4] = 0;
   out_3021660251758710384[5] = 0;
   out_3021660251758710384[6] = 1;
   out_3021660251758710384[7] = 0;
   out_3021660251758710384[8] = 0;
   out_3021660251758710384[9] = 1;
   out_3021660251758710384[10] = 0;
   out_3021660251758710384[11] = 0;
   out_3021660251758710384[12] = 0;
   out_3021660251758710384[13] = 0;
   out_3021660251758710384[14] = 0;
   out_3021660251758710384[15] = 0;
   out_3021660251758710384[16] = 0;
   out_3021660251758710384[17] = 0;
   out_3021660251758710384[18] = 0;
   out_3021660251758710384[19] = 0;
   out_3021660251758710384[20] = 0;
   out_3021660251758710384[21] = 0;
   out_3021660251758710384[22] = 0;
   out_3021660251758710384[23] = 0;
   out_3021660251758710384[24] = 0;
   out_3021660251758710384[25] = 1;
   out_3021660251758710384[26] = 0;
   out_3021660251758710384[27] = 0;
   out_3021660251758710384[28] = 1;
   out_3021660251758710384[29] = 0;
   out_3021660251758710384[30] = 0;
   out_3021660251758710384[31] = 0;
   out_3021660251758710384[32] = 0;
   out_3021660251758710384[33] = 0;
   out_3021660251758710384[34] = 0;
   out_3021660251758710384[35] = 0;
   out_3021660251758710384[36] = 0;
   out_3021660251758710384[37] = 0;
   out_3021660251758710384[38] = 0;
   out_3021660251758710384[39] = 0;
   out_3021660251758710384[40] = 0;
   out_3021660251758710384[41] = 0;
   out_3021660251758710384[42] = 0;
   out_3021660251758710384[43] = 0;
   out_3021660251758710384[44] = 1;
   out_3021660251758710384[45] = 0;
   out_3021660251758710384[46] = 0;
   out_3021660251758710384[47] = 1;
   out_3021660251758710384[48] = 0;
   out_3021660251758710384[49] = 0;
   out_3021660251758710384[50] = 0;
   out_3021660251758710384[51] = 0;
   out_3021660251758710384[52] = 0;
   out_3021660251758710384[53] = 0;
}
void h_10(double *state, double *unused, double *out_8232416234148166063) {
   out_8232416234148166063[0] = 9.8100000000000005*sin(state[1]) - state[4]*state[8] + state[5]*state[7] + state[12] + state[15];
   out_8232416234148166063[1] = -9.8100000000000005*sin(state[0])*cos(state[1]) + state[3]*state[8] - state[5]*state[6] + state[13] + state[16];
   out_8232416234148166063[2] = -9.8100000000000005*cos(state[0])*cos(state[1]) - state[3]*state[7] + state[4]*state[6] + state[14] + state[17];
}
void H_10(double *state, double *unused, double *out_9213737062496983753) {
   out_9213737062496983753[0] = 0;
   out_9213737062496983753[1] = 9.8100000000000005*cos(state[1]);
   out_9213737062496983753[2] = 0;
   out_9213737062496983753[3] = 0;
   out_9213737062496983753[4] = -state[8];
   out_9213737062496983753[5] = state[7];
   out_9213737062496983753[6] = 0;
   out_9213737062496983753[7] = state[5];
   out_9213737062496983753[8] = -state[4];
   out_9213737062496983753[9] = 0;
   out_9213737062496983753[10] = 0;
   out_9213737062496983753[11] = 0;
   out_9213737062496983753[12] = 1;
   out_9213737062496983753[13] = 0;
   out_9213737062496983753[14] = 0;
   out_9213737062496983753[15] = 1;
   out_9213737062496983753[16] = 0;
   out_9213737062496983753[17] = 0;
   out_9213737062496983753[18] = -9.8100000000000005*cos(state[0])*cos(state[1]);
   out_9213737062496983753[19] = 9.8100000000000005*sin(state[0])*sin(state[1]);
   out_9213737062496983753[20] = 0;
   out_9213737062496983753[21] = state[8];
   out_9213737062496983753[22] = 0;
   out_9213737062496983753[23] = -state[6];
   out_9213737062496983753[24] = -state[5];
   out_9213737062496983753[25] = 0;
   out_9213737062496983753[26] = state[3];
   out_9213737062496983753[27] = 0;
   out_9213737062496983753[28] = 0;
   out_9213737062496983753[29] = 0;
   out_9213737062496983753[30] = 0;
   out_9213737062496983753[31] = 1;
   out_9213737062496983753[32] = 0;
   out_9213737062496983753[33] = 0;
   out_9213737062496983753[34] = 1;
   out_9213737062496983753[35] = 0;
   out_9213737062496983753[36] = 9.8100000000000005*sin(state[0])*cos(state[1]);
   out_9213737062496983753[37] = 9.8100000000000005*sin(state[1])*cos(state[0]);
   out_9213737062496983753[38] = 0;
   out_9213737062496983753[39] = -state[7];
   out_9213737062496983753[40] = state[6];
   out_9213737062496983753[41] = 0;
   out_9213737062496983753[42] = state[4];
   out_9213737062496983753[43] = -state[3];
   out_9213737062496983753[44] = 0;
   out_9213737062496983753[45] = 0;
   out_9213737062496983753[46] = 0;
   out_9213737062496983753[47] = 0;
   out_9213737062496983753[48] = 0;
   out_9213737062496983753[49] = 0;
   out_9213737062496983753[50] = 1;
   out_9213737062496983753[51] = 0;
   out_9213737062496983753[52] = 0;
   out_9213737062496983753[53] = 1;
}
void h_13(double *state, double *unused, double *out_7562795773385764387) {
   out_7562795773385764387[0] = state[3];
   out_7562795773385764387[1] = state[4];
   out_7562795773385764387[2] = state[5];
}
void H_13(double *state, double *unused, double *out_6233934077091043185) {
   out_6233934077091043185[0] = 0;
   out_6233934077091043185[1] = 0;
   out_6233934077091043185[2] = 0;
   out_6233934077091043185[3] = 1;
   out_6233934077091043185[4] = 0;
   out_6233934077091043185[5] = 0;
   out_6233934077091043185[6] = 0;
   out_6233934077091043185[7] = 0;
   out_6233934077091043185[8] = 0;
   out_6233934077091043185[9] = 0;
   out_6233934077091043185[10] = 0;
   out_6233934077091043185[11] = 0;
   out_6233934077091043185[12] = 0;
   out_6233934077091043185[13] = 0;
   out_6233934077091043185[14] = 0;
   out_6233934077091043185[15] = 0;
   out_6233934077091043185[16] = 0;
   out_6233934077091043185[17] = 0;
   out_6233934077091043185[18] = 0;
   out_6233934077091043185[19] = 0;
   out_6233934077091043185[20] = 0;
   out_6233934077091043185[21] = 0;
   out_6233934077091043185[22] = 1;
   out_6233934077091043185[23] = 0;
   out_6233934077091043185[24] = 0;
   out_6233934077091043185[25] = 0;
   out_6233934077091043185[26] = 0;
   out_6233934077091043185[27] = 0;
   out_6233934077091043185[28] = 0;
   out_6233934077091043185[29] = 0;
   out_6233934077091043185[30] = 0;
   out_6233934077091043185[31] = 0;
   out_6233934077091043185[32] = 0;
   out_6233934077091043185[33] = 0;
   out_6233934077091043185[34] = 0;
   out_6233934077091043185[35] = 0;
   out_6233934077091043185[36] = 0;
   out_6233934077091043185[37] = 0;
   out_6233934077091043185[38] = 0;
   out_6233934077091043185[39] = 0;
   out_6233934077091043185[40] = 0;
   out_6233934077091043185[41] = 1;
   out_6233934077091043185[42] = 0;
   out_6233934077091043185[43] = 0;
   out_6233934077091043185[44] = 0;
   out_6233934077091043185[45] = 0;
   out_6233934077091043185[46] = 0;
   out_6233934077091043185[47] = 0;
   out_6233934077091043185[48] = 0;
   out_6233934077091043185[49] = 0;
   out_6233934077091043185[50] = 0;
   out_6233934077091043185[51] = 0;
   out_6233934077091043185[52] = 0;
   out_6233934077091043185[53] = 0;
}
void h_14(double *state, double *unused, double *out_6527835941710948675) {
   out_6527835941710948675[0] = state[6];
   out_6527835941710948675[1] = state[7];
   out_6527835941710948675[2] = state[8];
}
void H_14(double *state, double *unused, double *out_61128180536661912) {
   out_61128180536661912[0] = 0;
   out_61128180536661912[1] = 0;
   out_61128180536661912[2] = 0;
   out_61128180536661912[3] = 0;
   out_61128180536661912[4] = 0;
   out_61128180536661912[5] = 0;
   out_61128180536661912[6] = 1;
   out_61128180536661912[7] = 0;
   out_61128180536661912[8] = 0;
   out_61128180536661912[9] = 0;
   out_61128180536661912[10] = 0;
   out_61128180536661912[11] = 0;
   out_61128180536661912[12] = 0;
   out_61128180536661912[13] = 0;
   out_61128180536661912[14] = 0;
   out_61128180536661912[15] = 0;
   out_61128180536661912[16] = 0;
   out_61128180536661912[17] = 0;
   out_61128180536661912[18] = 0;
   out_61128180536661912[19] = 0;
   out_61128180536661912[20] = 0;
   out_61128180536661912[21] = 0;
   out_61128180536661912[22] = 0;
   out_61128180536661912[23] = 0;
   out_61128180536661912[24] = 0;
   out_61128180536661912[25] = 1;
   out_61128180536661912[26] = 0;
   out_61128180536661912[27] = 0;
   out_61128180536661912[28] = 0;
   out_61128180536661912[29] = 0;
   out_61128180536661912[30] = 0;
   out_61128180536661912[31] = 0;
   out_61128180536661912[32] = 0;
   out_61128180536661912[33] = 0;
   out_61128180536661912[34] = 0;
   out_61128180536661912[35] = 0;
   out_61128180536661912[36] = 0;
   out_61128180536661912[37] = 0;
   out_61128180536661912[38] = 0;
   out_61128180536661912[39] = 0;
   out_61128180536661912[40] = 0;
   out_61128180536661912[41] = 0;
   out_61128180536661912[42] = 0;
   out_61128180536661912[43] = 0;
   out_61128180536661912[44] = 1;
   out_61128180536661912[45] = 0;
   out_61128180536661912[46] = 0;
   out_61128180536661912[47] = 0;
   out_61128180536661912[48] = 0;
   out_61128180536661912[49] = 0;
   out_61128180536661912[50] = 0;
   out_61128180536661912[51] = 0;
   out_61128180536661912[52] = 0;
   out_61128180536661912[53] = 0;
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

void pose_update_4(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_4, H_4, NULL, in_z, in_R, in_ea, MAHA_THRESH_4);
}
void pose_update_10(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_10, H_10, NULL, in_z, in_R, in_ea, MAHA_THRESH_10);
}
void pose_update_13(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_13, H_13, NULL, in_z, in_R, in_ea, MAHA_THRESH_13);
}
void pose_update_14(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<3, 3, 0>(in_x, in_P, h_14, H_14, NULL, in_z, in_R, in_ea, MAHA_THRESH_14);
}
void pose_err_fun(double *nom_x, double *delta_x, double *out_207833252131504865) {
  err_fun(nom_x, delta_x, out_207833252131504865);
}
void pose_inv_err_fun(double *nom_x, double *true_x, double *out_5128581499076305372) {
  inv_err_fun(nom_x, true_x, out_5128581499076305372);
}
void pose_H_mod_fun(double *state, double *out_750146958085197188) {
  H_mod_fun(state, out_750146958085197188);
}
void pose_f_fun(double *state, double dt, double *out_6411414369012128450) {
  f_fun(state,  dt, out_6411414369012128450);
}
void pose_F_fun(double *state, double dt, double *out_503381519559238876) {
  F_fun(state,  dt, out_503381519559238876);
}
void pose_h_4(double *state, double *unused, double *out_8930625875686900431) {
  h_4(state, unused, out_8930625875686900431);
}
void pose_H_4(double *state, double *unused, double *out_3021660251758710384) {
  H_4(state, unused, out_3021660251758710384);
}
void pose_h_10(double *state, double *unused, double *out_8232416234148166063) {
  h_10(state, unused, out_8232416234148166063);
}
void pose_H_10(double *state, double *unused, double *out_9213737062496983753) {
  H_10(state, unused, out_9213737062496983753);
}
void pose_h_13(double *state, double *unused, double *out_7562795773385764387) {
  h_13(state, unused, out_7562795773385764387);
}
void pose_H_13(double *state, double *unused, double *out_6233934077091043185) {
  H_13(state, unused, out_6233934077091043185);
}
void pose_h_14(double *state, double *unused, double *out_6527835941710948675) {
  h_14(state, unused, out_6527835941710948675);
}
void pose_H_14(double *state, double *unused, double *out_61128180536661912) {
  H_14(state, unused, out_61128180536661912);
}
void pose_predict(double *in_x, double *in_P, double *in_Q, double dt) {
  predict(in_x, in_P, in_Q, dt);
}
}

const EKF pose = {
  .name = "pose",
  .kinds = { 4, 10, 13, 14 },
  .feature_kinds = {  },
  .f_fun = pose_f_fun,
  .F_fun = pose_F_fun,
  .err_fun = pose_err_fun,
  .inv_err_fun = pose_inv_err_fun,
  .H_mod_fun = pose_H_mod_fun,
  .predict = pose_predict,
  .hs = {
    { 4, pose_h_4 },
    { 10, pose_h_10 },
    { 13, pose_h_13 },
    { 14, pose_h_14 },
  },
  .Hs = {
    { 4, pose_H_4 },
    { 10, pose_H_10 },
    { 13, pose_H_13 },
    { 14, pose_H_14 },
  },
  .updates = {
    { 4, pose_update_4 },
    { 10, pose_update_10 },
    { 13, pose_update_13 },
    { 14, pose_update_14 },
  },
  .Hes = {
  },
  .sets = {
  },
  .extra_routines = {
  },
};

ekf_lib_init(pose)
