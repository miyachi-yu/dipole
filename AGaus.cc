#include "AGaus.hh"
#include <iostream>
#include <sstream>
#include <cmath>

using namespace std;

AGaus::AGaus(): A( 10.0 ), mean( 10.0 ), sigma( 4.0 ), asym( 1.0 ) {
}

AGaus::~AGaus() {
}

double AGaus::operator()( const double& x ){
  return A * exp( -0.5*pow( ( x-mean ) / ( ( x > mean ? asym : 1.0 ) * sigma ), 2.0 ) );
}

string AGaus::text(){
  ostringstream ostr;
  ostr << "#rho(r) = "<< A
       << " #font[02]{exp}#left( -#frac{1}{2} #left(#frac{r-"<<mean
       <<"}{("<<asym<<"#times)"<<sigma<<"}#right)^{2}#right)";
  return ostr.str();
}

ostream& operator<<( ostream& os, const AGaus& rho ){
  os << "A:" << setw(10) << rho.A
     << ", mean: " << setw(10) << rho.mean
     << ", sigma: " << setw(10) << rho.sigma
     << ", asymmetry: " << setw(10) << rho.asym;
  return os;
}

ClassImp( AGaus );
