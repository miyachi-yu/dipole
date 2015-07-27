#include "Fitter.hh"
#include "MyApplication.hh"
#include "AGaus.hh"
#include "NearestNeighbor.hh"

#include <TGraph.h>
#include <cmath>

using namespace std;

Fitter::Fitter() : TMinuit(), app_( NULL ), g_( NULL ),
		   tmin_( 0.0 ), tmax_( 0.0 ) {
  
  app_ = MyApplication::instance();
  int errflg;
  
  this->DefineParameter( 0, "amplitude", app_->amplitude(), 10.0,  0.0, 1.0E+6 );
  this->DefineParameter( 1, "mean",      app_->mean(),       0.5,  0.0, 1.0E+6 );
  
  AGaus *ag = dynamic_cast< AGaus* >( app_->density() );
  
  if( ag ){
    this->DefineParameter( 2, "sigmap", ag->asigma( true ),  0.5,  0.0, 1.0E+6 );
    this->DefineParameter( 3, "sigmam", ag->asigma( false ), 0.01, 0.0, 1.0E+6 );
  } else {
    this->DefineParameter( 2, "sigma",     app_->sigma(),      0.01, 0.0, 1.0E+6 );
  }
  
  //  this->DefineParameter( 3, "asym",      app_->asym(),       0.5,  0.0, 1.0E+6 );

}

Fitter::~Fitter() {
}

Int_t Fitter::Eval( Int_t npar, Double_t* grad,
		    Double_t& fval, Double_t* par, Int_t flag ){
  // set parameters
  app_->amplitude( par[ 0 ] );
  app_->mean(      par[ 1 ] );
  
  AGaus *ag = dynamic_cast< AGaus* >( app_->density() );
  if( ag ){
    ag->asigma( true,  par[ 2 ] );
    ag->asigma( false, par[ 3 ] );
    app_->update();
  }
  
  fval = 0.0;
  
  for( int i = 0; i < g_->GetN(); i++ ){
    double t, vraw;
    g_->GetPoint( i, t, vraw );
    if( ( tmin_ < tmax_ ) && ( t < tmin_ || t > tmax_ ) ) continue;
    fval += pow( vraw - app_->evalI( t ), 2.0 );
  }
}

void Fitter::fit( TGraph *g ){
  g_ = g;
  tmin_ = tmax_; // default mode
  this->Migrad();
}

void Fitter::fit( TGraph *g, const double& min, const double& max ){
  g_ = g;
  tmin_ = ( min < max ? min : max );
  tmax_ = ( min < max ? max : min );
  this->Migrad();
  tmin_ = tmax_; // back to default mode
}
