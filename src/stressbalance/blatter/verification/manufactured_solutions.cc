// DO NOT EDIT. This code was generated by a Python script.
#include <cmath>

#include "manufactured_solutions.hh"

namespace pism {

Vector2 blatter_xy_exact(double x, double y)
{
  double x0 = exp(x);
  double x1 = 2*M_PI*y;
  return {
    x0*sin(x1),
    x0*cos(x1)
  };
}

Vector2 blatter_xy_source(double x, double y, double B)
{
  double x0 = 2*M_PI*y;
  double x1 = cos(x0);
  double x2 = pow(x1, 2);
  double x3 = pow(M_PI, 2);
  double x4 = pow(M_PI, 3);
  double x5 = pow(M_PI, 4);
  double x6 = sin(x0);
  double x7 = pow(x6, 2);
  double x8 = pow(2, 2.0/3.0)*B*exp((1.0/3.0)*x)/pow(x2*(4*x3 + 1 + 4*M_PI) + x7*(16*x3 - 8*M_PI + 4), 4.0/3.0);
  return {
    -1.0/3.0*x6*x8*(x2*(6*x3 + 20*x4 + 72*x5 - 2 + 3*M_PI) + x7*(-48*x3 + 32*x4 + 96*x5 - 8 + 36*M_PI)),
    -1.0/6.0*x1*x8*(x2*(-12*x3 + 136*x4 + 192*x5 - 18*M_PI - 1) + x7*(96*x3 - 128*x4 + 384*x5 - 24*M_PI - 4))
  };
}

Vector2 blatter_xz_exact(double x, double z, double A, double rho, double g, double s_0, double alpha, double H, double beta) {

  return {
    -4*A*pow(alpha, 3)*pow(g, 3)*pow(rho, 3)*pow(x, 3)*(-pow(H, 4) + pow(-alpha*pow(x, 2) + s_0 - z, 4)) + 2*H*alpha*g*rho*x/beta,
    0.0
  };
}

Vector2 blatter_xz_source(double x, double z, double A, double rho, double g, double s_0, double alpha, double H, double beta) {
  double phi_1 = alpha*pow(x, 2) - s_0 + z;
  double phi_2 = 4*A*pow(alpha, 3)*pow(g, 3)*pow(rho, 3)*x;
  double phi_3 = 4*pow(phi_1, 5)*pow(phi_2, 2)*pow(x, 3);
  double phi_4 = -2*H*alpha*g*rho/beta + 8*alpha*pow(phi_1, 3)*phi_2*pow(x, 3) + 3*phi_2*x*(-pow(H, 4) + pow(phi_1, 4));
  double phi_5 = 48*pow(alpha, 2)*pow(phi_1, 2)*phi_2*pow(x, 4) + 56*alpha*pow(phi_1, 3)*phi_2*pow(x, 2) + 6*phi_2*(-pow(H, 4) + pow(phi_1, 4));
  double mu = (1.0/2.0)/cbrt(A*phi_1*phi_3*x + A*pow(phi_4, 2));

  return {
    (16.0/3.0)*A*pow(mu, 4)*(-6*pow(phi_1, 3)*phi_2*phi_3*pow(x, 3) - 18*pow(phi_1, 2)*phi_2*pow(phi_4, 2)*pow(x, 2) - 6*phi_1*phi_3*phi_5*x + 24*phi_3*phi_4*(2*alpha*pow(x, 2) + phi_1) - 2*pow(phi_4, 2)*phi_5),
    0.0
  };
}

Vector2 blatter_xz_source_bed(double x, double z, double A, double rho, double g, double s_0, double alpha, double H, double beta) {
  double phi_1 = alpha*pow(x, 2) - s_0 + z;
  double phi_2 = 4*A*pow(alpha, 3)*pow(g, 3)*pow(rho, 3)*x;
  double phi_3 = 4*pow(phi_1, 5)*pow(phi_2, 2)*pow(x, 3);
  double phi_4 = -2*H*alpha*g*rho/beta + 8*alpha*pow(phi_1, 3)*phi_2*pow(x, 3) + 3*phi_2*x*(-pow(H, 4) + pow(phi_1, 4));
  double mu = (1.0/2.0)/cbrt(A*phi_1*phi_3*x + A*pow(phi_4, 2));
  double n_x = -2*alpha*x/sqrt(4*pow(alpha, 2)*pow(x, 2) + 1);
  double n_z = -1/sqrt(4*pow(alpha, 2)*pow(x, 2) + 1);

  return {
    2*H*alpha*g*rho*x - beta*phi_2*pow(x, 2)*(-pow(H, 4) + pow(phi_1, 4)) - 4*mu*n_x*phi_4 - 4*mu*n_z*pow(phi_1, 3)*phi_2*pow(x, 2),
    0.0
  };
}

Vector2 blatter_xz_source_surface(double x, double z, double A, double rho, double g, double s_0, double alpha, double H, double beta) {
  double phi_1 = alpha*pow(x, 2) - s_0 + z;
  double phi_2 = 4*A*pow(alpha, 3)*pow(g, 3)*pow(rho, 3)*x;
  double phi_3 = 4*pow(phi_1, 5)*pow(phi_2, 2)*pow(x, 3);
  double phi_4 = -2*H*alpha*g*rho/beta + 8*alpha*pow(phi_1, 3)*phi_2*pow(x, 3) + 3*phi_2*x*(-pow(H, 4) + pow(phi_1, 4));
  double mu = (1.0/2.0)/cbrt(A*phi_1*phi_3*x + A*pow(phi_4, 2));
  double n_x = 2*alpha*x/sqrt(4*pow(alpha, 2)*pow(x, 2) + 1);
  double n_z = pow(4*pow(alpha, 2)*pow(x, 2) + 1, -1.0/2.0);

  return {
    -4*mu*n_x*phi_4 - 4*mu*n_z*pow(phi_1, 3)*phi_2*pow(x, 2),
    0.0
  };
}

Vector2 blatter_xz_cfbc_exact(double x, double z, double B, double L, double rho_i, double rho_w, double g)
{
  return {
    (1.0/2.0)*L*g*z*(rho_i - rho_w)*sin(M_PI*x/L)/(M_PI*B),
    0
  };
}

Vector2 blatter_xz_cfbc_source(double x, double z, double L, double rho_i, double rho_w, double g)
{
  double x0 = M_PI/L;
  return {
    -g*x0*z*(rho_i - rho_w)*sin(x*x0),
    0
  };
}

Vector2 blatter_xz_cfbc_surface(double x, double L, double rho_i, double rho_w, double g)
{
  return {
    (1.0/4.0)*L*g*(rho_i - rho_w)*sin(M_PI*x/L)/M_PI,
    0
  };
}

Vector2 blatter_xz_cfbc_base(double x, double L, double rho_i, double rho_w, double g)
{
  return {
    -1.0/4.0*L*g*(rho_i - rho_w)*sin(M_PI*x/L)/M_PI,
    0
  };
}

Vector2 blatter_xz_halfar_exact(double x, double z, double H_0, double R_0, double rho_i, double g, double B)
{
  double x0 = 1 - pow(x, 4.0/3.0)/pow(R_0, 4.0/3.0);
  double x1 = pow(x0, 12.0/7.0);
  return {
    (32.0/343.0)*pow(H_0, 3)*pow(g, 3)*pow(rho_i, 3)*x*(pow(H_0, 4)*x1 - pow(H_0*pow(x0, 3.0/7.0) - z, 4))/(pow(B, 3)*pow(R_0, 4)*x1),
    0
  };
}

Vector2 blatter_xz_halfar_source(double x, double z, double H_0, double R_0, double rho_i, double g, double B)
{
  double x0 = pow(x, 2);
  double x1 = pow(R_0, -8);
  double x2 = pow(R_0, -4.0/3.0);
  double x3 = pow(x, 4.0/3.0);
  double x4 = -x2*x3 + 1;
  double x5 = pow(x4, -24.0/7.0);
  double x6 = pow(H_0, 6);
  double x7 = H_0*pow(x4, 3.0/7.0) - z;
  double x8 = pow(g, 6)*pow(rho_i, 6)/pow(B, 6);
  double x9 = x6*pow(x7, 6)*x8;
  double x10 = x1*x5*x9;
  double x11 = pow(H_0, 4);
  double x12 = pow(x4, 12.0/7.0);
  double x13 = pow(H_0, 3);
  double x14 = pow(g, 3);
  double x15 = pow(rho_i, 3);
  double x16 = x14*x15/pow(B, 3);
  double x17 = x13*x16;
  double x18 = x17*(x11*x12 - pow(x7, 4));
  double x19 = 1/(pow(R_0, 4)*x12);
  double x20 = x18*x19;
  double x21 = pow(R_0, -16.0/3.0);
  double x22 = x21*x3;
  double x23 = pow(x4, -19.0/7.0);
  double x24 = x18*x23;
  double x25 = x22*x24;
  double x26 = x11*x2*pow(x4, 5.0/7.0);
  double x27 = cbrt(x);
  double x28 = (16.0/7.0)*x27;
  double x29 = pow(x7, 3);
  double x30 = H_0*x29;
  double x31 = x2*x30/pow(x4, 4.0/7.0);
  double x32 = -x26*x28 + x28*x31;
  double x33 = x17*x19;
  double x34 = x32*x33;
  double x35 = (32.0/343.0)*x*x34 + (32.0/343.0)*x20 + (512.0/2401.0)*x25;
  double x36 = (4096.0/117649.0)*x0*x10 + pow(x35, 2);
  double x37 = pow(x36, -1.0/3.0);
  double x38 = pow(x7, 2);
  double x39 = x*x13*x14*x15*x19/pow(B, 2);
  double x40 = pow(x36, -4.0/3.0);
  double x41 = pow(x7, 5)*x8;
  double x42 = (1.0/3.0)*x35;
  double x43 = pow(x, 5.0/3.0)*x18/(pow(R_0, 20.0/3.0)*pow(x4, 26.0/7.0));
  double x44 = x21*x24*x27;
  double x45 = x17*x22*x23*x32;
  double x46 = pow(x, 2.0/3.0);
  double x47 = x46/pow(R_0, 8.0/3.0);
  double x48 = (16.0/21.0)/x46;
  double x49 = x*x33*(-192.0/49.0*pow(H_0, 2)*x38*x47/pow(x4, 8.0/7.0) + (320.0/147.0)*x11*x47/pow(x4, 2.0/7.0) - x26*x48 + (256.0/147.0)*x30*x47/pow(x4, 11.0/7.0) + x31*x48);
  double x50 = (1.0/2.0)*B;
  double x51 = (128.0/343.0)*x34;
  double x52 = pow(x, 7.0/3.0)/pow(R_0, 28.0/3.0);
  return {
    (64.0/343.0)*x29*x39*x40*((8192.0/117649.0)*x0*x1*x41*x5*x6 - x42*(-3072.0/2401.0*x11*x16*x22*x38/pow(x4, 16.0/7.0) + (4096.0/2401.0)*x17*x22*x23*x29 + (256.0/343.0)*x29*x33)) - 192.0/343.0*x37*x38*x39 + x37*x50*((256.0/343.0)*x34 + (155648.0/50421.0)*x43 + (2048.0/1029.0)*x44 + (4096.0/2401.0)*x45 + (128.0/343.0)*x49) + x40*x50*(x*x51 + (128.0/343.0)*x20 + (2048.0/2401.0)*x25)*((32768.0/823543.0)*pow(H_0, 7)*x41*x52/pow(x4, 4) - 8192.0/352947.0*x*x10 - x42*((77824.0/50421.0)*x43 + (1024.0/1029.0)*x44 + (2048.0/2401.0)*x45 + (64.0/343.0)*x49 + x51) - 131072.0/2470629.0*x52*x9/pow(x4, 31.0/7.0)),
    0
  };
}

} // end of namespace pism
