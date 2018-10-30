#ifndef checks_cpp
#define checks_cpp

#include <Rcpp.h>

using namespace Rcpp;

void isPD(double PD)
{
  if ((PD < 0) | (PD > 1))
  {
    throw std::invalid_argument("PD must be between 0 and 1");
  }
}

void isEAD(double EAD)
{
  if (EAD < 0)
  {
    throw std::invalid_argument("EAD must be positive");
  }
}

void isLGD(double LGD)
{
  if ((LGD < 0) | (LGD > 1))
  {
    throw std::invalid_argument("LGD must be between 0 and 1");
  }
}

void isRho(double R)
{
  if ((R < 0) | (R > 1))
  {
    throw std::invalid_argument("Rho must be between 0 and 1");
  }
}

#endif