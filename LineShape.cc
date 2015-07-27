#include "LineShape.hh"
#include "Density.hh"
#include <Tranform/RTransform.hh>

#include "AGaus.hh"

double LineShape::operator()( double* x, double *p ){
  
  rho_->amplitude( p[ 0 ] );
  rho_->mean( p[ 1 ] );
  
  AGaus *ag = dynamic_cast< AGaus* >( rho_ );
  if( ag ){
    ag->asigma( true,  p[ 2 ] );
    ag->asigma( false, p[ 3 ] );
  }
  
  rT_->upper( rho_->upper() );
  rT_->lower( rho_->lower() );

  return (*rT_)( x[ 0 ] );
  
}


ClassImp( LineShape );
