#include "MixedDensity.hh"

using namespace std;

MixedDensity::MixedDensity() :
  vector< Transform::RealFunction* >( 0 ) {
}

MixedDensity::~MixedDensity(){
}

double MixedDensity::operator()( const double& x ){
  double v = 0.0;
  for( int i = 0; i < this->size(); i++ ){
    v += (*( (*this)[i] ) )( x );
  }
  return v;
}

ClassImp( MixedDensity );
