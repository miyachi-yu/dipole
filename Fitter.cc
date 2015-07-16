#include "Fitter.hh"
#include "MyApplication.hh"

#include <TGraph.h>
#include <cmath>

using namespace std;

Fitter::Fitter() : TMinuit(), app_( NULL ) {
  app_ = MyApplication::instance();
  int errflg;
  
  this->mnparm( 0, "amplitude", app_->amplitude(), 1.0, 0.0, 1.0E+6, errflg );
  this->mnparm( 1, "mean",      app_->mean(),      1.0, 0.0, 1.0E+6, errflg );
  this->mnparm( 2, "sigma",     app_->sigma(),     1.0, 0.0, 1.0E+6, errflg );
  this->mnparm( 3, "asym",      app_->asym(),      1.0, 0.0, 1.0E+6, errflg );
  
  
}

Fitter::~Fitter() {
}

Int_t Fitter::Eval( Int_t npar, Double_t* grad,
		    Double_t& fval, Double_t* par, Int_t flag ){
  // set parameters
  app_->amplitude( par[ 0 ] );
  app_->mean(      par[ 1 ] );
  app_->sigma(     par[ 2 ] );
  app_->asym(      par[ 3 ] );

  fval = 0.0;

  for( int i = 0; i < g_->GetN(); i++ ){
    double t, vraw;
    g_->GetPoint( i, t, vraw );
    fval += pow( vraw - app_->evalI( t ), 2.0 );
  }
}

void Fitter::fit( TGraph *g ){
  g_ = g;
  this->Migrad();
}
