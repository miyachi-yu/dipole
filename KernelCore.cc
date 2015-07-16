#include "KernelCore.hh"

#include <cmath>
#include <sstream>
#include <algorithm>

using namespace std;

KernelCore::KernelCore() {
}

KernelCore::~KernelCore() {
}

double KernelCore::f( const double& x ){
  // x must be -1 < x < 2
  if( x < -1.0 || x > 2.0 ) return 0.0;
  double v = x + 1.0;
  return ( v > 0.0 ? 1.0 / sqrt( v/3.0 ) : 0.0 );
}

double KernelCore::ftilde( const double& x ){
  return this->f( x ) + this->f( - x );
}

double KernelCore::operator()( const double& r, const double& t ){
  return this->ftilde( - t * pow( r, 3.0 ) / 1.395 );
}

string KernelCore::text(){
  ostringstream ost;
  ost << "#left(#frac{-(tr^{3}/1.395)+1}{3}#right)";
  return ost.str();
}

ClassImp( KernelCore );
