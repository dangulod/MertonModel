#ifndef wholesale__h
#define wholesale__h

#include "creditRisk.h"
#include <random>
#include <thread>
#include <mutex>

class Bucket
{
private:
  double EAD, LGD, PD, nPD, rho, BIdio;
  
public:
  Bucket();
  Bucket(double EAD, double PD, double LGD, double rho);
  ~Bucket();
  
  double getEAD();
  void setEAD(double value);
  
  double getLGD();
  void setLGD(double value);
  
  double getPD();
  double getnPD();
  void setPD(double value);
  
  double getRho();
  void setRho(double value);
  double getIdio();
  
  double loss(double Sn);
};

class Wholesale 
{
private:
  std::vector<Bucket> counterparty;
  
public:
  Wholesale();
  ~Wholesale();
  
  void addCounterparty(Bucket value);
  std::vector<Bucket> getCounterparties();
  Bucket getCounterparty(int i);
  
  std::vector<double> getEADs();
  std::vector<double> getLGDs();
  std::vector<double> getPDs();
  std::vector<double> getnPDs();
  std::vector<double> getRhos();
  std::vector<double> getIdios();
  
  int getN();
  
  std::vector<double> loss(std::vector<double> rnd);
  
};

#endif