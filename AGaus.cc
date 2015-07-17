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
  double area = 0.5 * sqrt( 2.0 * M_PI ) * sigma * ( 1.0 + asym ) ;
  return A * exp( -0.5*pow( ( x-mean ) / ( ( x > mean ? asym : 1.0 ) * sigma ), 2.0 ) ) / area;
}

string AGaus::text(){
  ostringstream ostr;
  ostr << "#rho(r) = #frac{" << A << "}{area}"
       << " #font[02]{exp}#left( -#frac{1}{2} #left(#frac{r-"<<mean
       <<"}{("<<asym<<"#times)"<<sigma<<"}#right)^{2}#right)";
  return ostr.str();
}

double AGaus::upper() {
  return mean + 4.0 * asym * sigma;
}

double AGaus::lower() {
  double v = mean - 4.0 * sigma;
  return ( v > 0.0 ? v : 0.0 );
}

ostream& operator<<( ostream& os, const AGaus& rho ){
  os << "A:" << setw(10) << rho.A
     << ", mean: " << setw(10) << rho.mean
     << ", sigma: " << setw(10) << rho.sigma
     << ", asymmetry: " << setw(10) << rho.asym;
  return os;
}

ClassImp( AGaus );
