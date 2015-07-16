#include "DipoleKernel.hh"

#include <cmath>
#include <sstream>
#include <algorithm>

using namespace std;

DipoleKernel::DipoleKernel() : core_(), lines_( 0 ) {
}

DipoleKernel::~DipoleKernel() {
}

/*!
  Unit of the given r is nm and unit of return value is mT.
  Constant term, -1.395, corresponds,
  {\mu_0 \over 4\pi}{ 3\mu_e \over 2 } = -1.395E-30 T/m^3 = -1.395 mT/(nm)^3 
*/
double DipoleKernel::localField( const double& r ){
  return r != 0.0 ? -1.395 * pow( r, -3.0 ) : 0.0 ; 
}

double DipoleKernel::weight( const double& r ){
  const double c = M_PI / 4.185;
  return c * pow( r, 5.0 );
}

// actual definition of the kernel functional form.
// r in nm
// t() in mT
double DipoleKernel::eval( const double& r ){
  if( ! ( r > 0.0 ) ) return 0.0;
  return this->weight( r ) * this->core( r, this->t() );
}

double DipoleKernel::core( const double& r, const double& t ){
  if( lines_.size() == 0 ) return core_( r, t );
  double fv = 0.0;
  for( int i = 0; i < lines_.size(); i++ ){
    fv += core_( r, t - lines_[ i ] );
  }
  return fv;
}

string DipoleKernel::text(){
  ostringstream ost;
  ost << "#tilde{K}(r,t)=#sum_{i}#left(#frac{1#pm((t-H_{i})/A(r))}{3}#right)^{-#frac{1}{2}}";
  ost << " where H_{i} =";
  if( lines_.size() == 0 ){
    ost << "0";
  } else {
    for( int i = 0; i < lines_.size(); i++ ){
      ost << lines_[ i ];
      if( i != lines_.size() - 1 ) ost << ", ";
    }
  }
  return ost.str();
}

void DipoleKernel::offset( const double& v ){
  if( ! ( v > 0.0 ) ) return;
  if( find( lines_.begin(), lines_.end(), v ) == lines_.end() )
    lines_.push_back( v );
}

double DipoleKernel::offset(){
  if( lines_.size() == 0 ) return 0.0;
  if( lines_.size() == 1 ) return lines_[ 0 ];
  double v = 0.0;
  for( int i = 0; i < lines_.size(); i++ ) v += lines_[ i ];
  return v / lines_.size();
}

ClassImp( DipoleKernel );
