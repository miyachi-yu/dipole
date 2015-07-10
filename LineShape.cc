#include "LineShape.hh"
#include "AGaus.hh"
#include <Tranform/RTransform.hh>

double LineShape::operator()( double* x, double *p ){

  rho_->A     = p[ 0 ];
  rho_->mean  = p[ 1 ];
  rho_->sigma = p[ 2 ];
  rho_->asym  = p[ 3 ];
  base_ = p[ 4 ];
  return (*rT_)( x[ 0 ] ) + base_;
  
}


ClassImp( LineShape );
