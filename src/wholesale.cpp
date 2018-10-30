#ifndef wholesale__cpp
#define wholesale__cpp

#include "wholesale.h"

static std::normal_distribution<double> normal(0, 1);
static std::mt19937_64 generator(1234);

double merton(double EAD, double nPD, double LGD, double rho, double BIdio, double Sn)
{
  double Y = sqrt(rho) * Sn + BIdio * normal(generator);
  return (nPD >= Y) ? EAD * LGD : 0;
}

/* Bucket class */

Bucket::Bucket() {  };

Bucket::~Bucket() {  };

Bucket::Bucket(double EAD, double PD, double LGD, double rho)
{
  setEAD(EAD);
  setLGD(LGD);
  setPD(PD);
  setRho(rho);
  this->BIdio = sqrt( 1 - this->rho );
  
}

double Bucket::getEAD()
{
  return this->EAD;
}

void Bucket::setEAD(double value)
{
  isEAD(value);
  this->EAD = value;
}

double Bucket::getLGD()
{
  return this->LGD;
}

void Bucket::setLGD(double value)
{
  isLGD(value);
  this->LGD = value;
}

double Bucket::getPD()
{
  return this->PD;
}

double Bucket::getnPD()
{
  return this->nPD;
}

void Bucket::setPD(double value)
{
  isPD(value);
  this->PD = value;
  this->nPD = qnor(PD, 1, 0);
}
  
double Bucket::getRho()
{
  return this->rho;
}

double Bucket::getIdio()
{
  return this->BIdio;
}

void Bucket::setRho(double value)
{
  isRho(value);
  this->rho = value;
}

double Bucket::loss(double rnd)
{
  return merton(this->EAD, this->nPD, this->LGD, this->rho, this->BIdio, rnd);
}

RCPP_EXPOSED_CLASS(Bucket)

RCPP_MODULE(Bucket) {
  using namespace Rcpp;
  Rcpp::class_<Bucket>( "Bucket" )
    .constructor("Construct an empty object of class Bucket")
    .constructor<double, double, double, double>("Create an object of class bucket")
    .method( "getEAD", &Bucket::getEAD, "Returns")
    .method( "setEAD", &Bucket::setEAD, "Returns")
    .method( "getLGD", &Bucket::getLGD, "Returns")
    .method( "setLGD", &Bucket::setLGD, "Returns")
    .method( "getPD", &Bucket::getPD, "Returns")
    .method( "getnPD", &Bucket::getnPD, "Returns")
    .method( "setPD", &Bucket::setPD, "Returns")
    .method( "getRho", &Bucket::getRho, "Returns")
    .method( "getIdio", &Bucket::getIdio, "Returns")
    .method( "setRho", &Bucket::setRho, "Returns")
    .method( "loss", &Bucket::loss, "Returns")
  ;
}

/* Wholesale class */

Wholesale::Wholesale() {  };

Wholesale::~Wholesale() {  };

int Wholesale::getN() 
{
  return this->counterparty.size();
}

void Wholesale::addCounterparty(Bucket value)
{
  this->counterparty.push_back(value);
}
  
std::vector<Bucket> Wholesale::getCounterparties()
{
  return this->counterparty;
}
  
Bucket Wholesale::getCounterparty(int i)
{
  if (i > getN()) throw std::invalid_argument(std::string("'i' can not exceed ") + std::to_string(getN()));
  if (i < 1) throw std::invalid_argument(std::string("i must be greater than 0"));
  
  return this->counterparty[i - 1];
}

std::vector<double> Wholesale::getEADs()
{
  int n(getN());
  std::vector<double> values(n);
  
  for (int i = 0; i < n; i++)
  {
    values[i] = getCounterparty(i + 1).getEAD();
  }
  
  return values;
}

std::vector<double> Wholesale::getPDs()
{
  int n(getN());
  std::vector<double> values(n);
  
  for (int i = 0; i < n; i++)
  {
    values[i] = getCounterparty(i + 1).getPD();
  }
  
  return values;
}

std::vector<double> Wholesale::getnPDs()
{
  int n(getN());
  std::vector<double> values(n);
  
  for (int i = 0; i < n; i++)
  {
    values[i] = getCounterparty(i + 1).getnPD();
  }
  
  return values;
}



std::vector<double> Wholesale::getLGDs()
{
  int n(getN());
  std::vector<double> values(n);
  
  for (int i = 0; i < n; i++)
  {
    values[i] = getCounterparty(i + 1).getLGD();
  }
  
  return values;
}

std::vector<double> Wholesale::getRhos()
{
  int n(getN());
  std::vector<double> values(n);
  
  for (int i = 0; i < n; i++)
  {
    values[i] = getCounterparty(i + 1).getRho();
  }
  
  return values;
}

std::vector<double> Wholesale::getIdios()
{
  int n(getN());
  std::vector<double> values(n);
  
  for (int i = 0; i < n; i++)
  {
    values[i] = getCounterparty(i + 1).getIdio();
  }
  
  return values;
}

void parallelmerton(int id, int n, std::vector<double> *loss, std::vector<double> EAD, std::vector<double> nPD, std::vector<double> LGD, std::vector<double> rho, std::vector<double> BIdio, std::vector<double> Sn)
{
  int i(id);
  int core(std::thread::hardware_concurrency());
  int l(LGD.size());

  while (i < n)
  {
    for (int j = 0; j < l; j++)
    {
      (*loss)[i] += merton(EAD[j], nPD[j], LGD[j], rho[j], BIdio[j], Sn[i]);
    }
    i += core;
  }
}


std::vector<double> Wholesale::loss(std::vector<double> Sn)
{
  int l(Sn.size());
  std::vector<double> losses(l);
  std::vector<double> LGD(getLGDs());
  std::vector<double> nPD(getnPDs());
  std::vector<double> EAD(getEADs());
  std::vector<double> rho(getRhos());
  std::vector<double> BIdio(getIdios());
  
  int core(std::thread::hardware_concurrency());

  std::thread *threads = new std::thread[core];

  for (int i = 0; i < core; i++)
  {
    threads[i] = std::thread(parallelmerton, i, l, &losses, EAD, nPD, LGD, rho, BIdio, Sn);
  }

  for (int i = 0; i < core; i++)
  {
    threads[i].join();
  }

  delete [] threads;

  return losses;

}


RCPP_EXPOSED_CLASS(Wholesale)
  
RCPP_MODULE(Wholesale) {
  using namespace Rcpp;
  Rcpp::class_<Wholesale>( "Wholesale" )
    .constructor("Construct an empty object of class Wholesale")
      .method( "addCounterparty", &Wholesale::addCounterparty, "Returns")
      .method( "getN", &Wholesale::getN, "Returns")
      .method( "getCounterparties", &Wholesale::getCounterparties, "Returns")
      .method( "getCounterparty", &Wholesale::getCounterparty, "Returns")
      .method( "getPDs", &Wholesale::getPDs, "Returns")
      .method( "getnPDs", &Wholesale::getnPDs, "Returns")
      .method( "getEADs", &Wholesale::getEADs, "Returns")
      .method( "getLGDs", &Wholesale::getLGDs, "Returns")
      .method( "getRhos", &Wholesale::getRhos, "Returns")
      .method( "getIdios", &Wholesale::getIdios, "Returns")
      .method( "loss", &Wholesale::loss, "Returns")
      ;
}

#endif