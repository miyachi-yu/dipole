#include "NearestNeighbor.hh"

#include <cmath>
#include <sstream>

using namespace std;

NearestNeighbor::NearestNeighbor() :
  a_( 10.0 ), rho_( 0.17 )
{
}

NearestNeighbor::~NearestNeighbor(){
}

double NearestNeighbor::operator()( const double& x ){
  const double p4 = 4.0 * M_PI;
  double rp4 = rho_ * p4;
  return a_ * rp4 * pow( x, 2.0 ) * exp( - rp4 * pow( x, 3.0 ) / 3.0 );
}

double NearestNeighbor::upper() const {
  return 3.0 * this->mean() ;
}

double NearestNeighbor::lower() const {
  return 0.0;
}


void NearestNeighbor::amplitude( const double& v ){
  a_ = v;
}

void NearestNeighbor::mean( const double& v ){
  rho_  = ( v > 0.0 ?  pow( 0.554 / v , 3 ) : 1.0E+99 ); 
}

double NearestNeighbor::amplitude() const { return a_; }

double NearestNeighbor::mean() const {
  return 0.554 / pow( rho_, 1.0 / 3 );
}

double NearestNeighbor::sigma() const {
  return 0.20 / pow( rho_, 1.0 / 3 );
}

double NearestNeighbor::asigma( const bool& plus ) const {
  return this->sigma();
}

string NearestNeighbor::text() const {
  ostringstream ostr;
  ostr << a_ 
       << "4 #pi " 
       << rho_  << "r^{2} e^{ - #frac{4}{3} #pi " << rho_ << " r^3" << endl;;
  return ostr.str();
}


ClassImp( NearestNeighbor );
