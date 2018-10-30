#ifndef creditRisk__h
#define creditRisk__h

#include <RcppArmadillo.h>

// [[Rcpp::depends(RcppArmadillo)]]
using namespace Rcpp;

void isPD(double PD);
void isEAD(double EAD);
void isLGD(double LGD);
void isRho(double R);

double qnor(double p, int lower_tail, int log_p);
double pnor(double x, double mu, double sigma, int lower_tail, int log_p);

#endif