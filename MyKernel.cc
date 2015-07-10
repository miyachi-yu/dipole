#include "MyKernel.hh"

#include <cmath>

MyKernel::MyKernel() : offset( 0.0 ) {
}

MyKernel::~MyKernel() {
}

double MyKernel::finv( const double& x ){
  double v = x + 1.0;
  if( !( v > 0.0 ) ) return 0.0; 
  v = sqrt( v / 3.0 );
  return fabs( v ) > 1.0 ? 0.0 : acos( v ) ;
}

/*!
  Unit of the given r is nm and unit of return value is mT.
  Constant term, -1.395, corresponds,
  {\mu_0 \over 4\pi}{ 3\mu_e \over 2 } = -1.395E-30 T/m^3 = -1.395 mT/(nm)^3 
*/
double MyKernel::localField( const double& r ){
  return r != 0.0 ? -1.395 * pow( r, -3.0 ) : 0.0 ; 
}

// actual definition of the kernel functional form.
// r in nm
// t() in mT
double MyKernel::eval( const double& r ){
  double v = ( r == 0 ? 0.0 :
	       ( this->t() - this->offset ) / this->localField( r ) );
  return 4.0 * M_PI * pow( r, 2.0 ) *
    ( this->finv( v ) + this->finv( -v ) ); 
}
