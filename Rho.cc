#include "Rho.hh"
#include <iostream>
#include <sstream>
#include <cmath>

using namespace std;

Rho::Rho(): A( 10.0 ), mean( 10.0 ), sigma( 4.0 ) {
}

Rho::~Rho() {
}

double Rho::operator()( const double& x ){
  return A * exp( -0.5*pow( ( x-mean ) / sigma, 2.0 ) );
}

string Rho::text(){
  ostringstream ostr;
  ostr << "#rho(r) = "<< A
       << " #font[02]{exp}#left( -#frac{1}{2} #left(#frac{r-"<<mean
       <<"}{"<<sigma<<"}#right)^{2}#right)";
  return ostr.str();
}

ostream& operator<<( ostream& os, const Rho& rho ){
  os << "A:" << setw(10) << rho.A
     << ", mean: " << setw(10) << rho.mean
     << ", sigma: " << setw(10) << rho.sigma;
  return os;
}

