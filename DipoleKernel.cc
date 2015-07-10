#include "DipoleKernel.hh"

#include <cmath>
#include <sstream>

using namespace std;

DipoleKernel::DipoleKernel() : offset( 0.0 ) {
}

DipoleKernel::~DipoleKernel() {
}

double DipoleKernel::finv( const double& x ){
  // x must be -1 < x < 2
  if( x < -1.0 || x > 2.0 ) return 0.0;
  double v = x + 1.0;
  return ( v > 0.0 ? 1.0 / sqrt( v/3.0 ) : 0.0 );
}

/*!
  Unit of the given r is nm and unit of return value is mT.
  Constant term, -1.395, corresponds,
  {\mu_0 \over 4\pi}{ 3\mu_e \over 2 } = -1.395E-30 T/m^3 = -1.395 mT/(nm)^3 
*/
double DipoleKernel::localField( const double& r ){
  return r != 0.0 ? -1.395 * pow( r, -3.0 ) : 0.0 ; 
}

// actual definition of the kernel functional form.
// r in nm
// t() in mT
double DipoleKernel::eval( const double& r ){
  double lf = this->localField( r );
  double v = ( r == 0 ? 0.0 : ( this->t() - this->offset ) / lf );
  return M_PI * pow( r, 2.0 ) / 3.0 / fabs( lf ) *
    ( this->finv( v ) + this->finv( -v ) ); 
}

string DipoleKernel::text(){
  ostringstream ost;
  ost << "K(r,t) = #frac{1}{6|A(r)|}#left(#frac{((H_{D}-"
      << this->offset
      << ")/A(r))+1}{3}#right)^{-#frac{1}{2}}";
  return ost.str();
}
