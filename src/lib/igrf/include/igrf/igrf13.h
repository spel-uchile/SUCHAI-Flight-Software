

#ifndef _IGRF13_H
int TransMagaxisToECI(const double* mag, double* pos, double lonrad, double thetarad, double gmst);
int RotationX(const double* bfr, double* aft, double theta);
int RotationY(const double* bfr, double* aft, double theta);
int RotationZ(const double* bfr, double* aft, double theta);
void IgrfCalc(double decyear, double latrad, double lonrad, double altm, double side, double* mag);
void read_model();
void calc_mag_coords();
int interpsh(double date, double dte1, int nmax1, double dte2,int nmax2, int gh);
int   extrapsh(double date, double dte1, int   nmax1, int   nmax2, int   gh);
int shval3(int igdgc, double flat, double flon, double elev, int nmax, int gh, int iext, double ext1, double ext2, double ext3);
int   dihf(int gh);
int getshc(char file[], int iflag, long int strec, int nmax_of_gh, int gh);
#define _IGRF13_H

#endif //_IGRF13_H
