#include "MyApplication.hh"
#include "DipoleKernel.hh"
#include "AGaus.hh"
#include "ESR.hh"
#include "LineShape.hh"

#include <Utility/Arguments.hh>
#include <Tranform/RealFunction.hh>
#include <Tranform/GridIntegration.hh>

#include <iostream>
#include <iomanip>
#include <sstream>

#include <TROOT.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TAxis.h>
#include <TLatex.h>
#include <TLine.h>
#include <TH1.h>
#include <TLegend.h>
#include <TCanvas.h>

using namespace std;

MyApplication* MyApplication::singleton_ = NULL;

MyApplication::MyApplication( int argc, char* argv[] ) :
  TRint( "MyApplication", &argc, argv ),
  k_( NULL ),
  rho_( NULL ),
  rT_( NULL ),
  line_( new TLine ),
  latex_( new TLatex ),
  c_( NULL ),
  tRange_( 2, 0.0 ),
  tstep_( 0.0 ), nT_( 300 ),
  I0_( 1.0 ),
  gESR_( NULL )
{
  Utility::Arguments& args = Utility::Arguments::ref();

  cout << "// --------------------------------------------------------- // " << endl;
  cout << "   You can obtain my instance via" << endl;
  cout << "   MyApplication *app = MyApplication::instance();" << endl;
  cout << "     OR   " << endl;
  cout << "   MyApplication *app = dynamic_cast< MyApplication* >( gROOT->GetApplcation() );" << endl;
  cout << "// --------------------------------------------------------- // " << endl;
  

  singleton_ = this;
  k_   = new DipoleKernel();
  rho_ = new AGaus;
  rT_ = new Transform::RTransform();
  rT_->precision( 0.0001 );
  rT_->nLeg( 4, 8 );
  
  rT_->kernel( k_ );
  rT_->integrand( rho_ );
  
  latex_->SetTextFont( 32 );
  latex_->SetTextSize( 0.03 );
  
  if( args.hasOpt( "toffset" ) ) k_->offset( args.get( "toffset", 0.0 ) );
  
  this->update();
  
  tRange_[ 0 ] = args.get( "tmin", tRange_[ 0 ] );
  tRange_[ 1 ] = args.get( "tmax", tRange_[ 1 ] );
  
  if( args.hasOpt( "tstep" ) ){
    tstep_ = args.get( "tstep", tstep_ );
    nT_ = ( tRange_[ 1 ] - tRange_[ 0 ] ) / tstep_;
  }
  
}

MyApplication::~MyApplication(){
  delete k_;
  delete rho_;
  delete rT_;
  delete line_;
  delete latex_;
  if( c_ ) delete c_;
  if( gESR_ ) { delete gESR_; }
}

void MyApplication::drawRho(){

  if( c_ ) c_->cd( 1 );
  
  int nx = 300;
  double dd = ( rT_->upper() - rT_->lower() ) / nx;
  
  TGraph *g = new TGraph( nx );
  for( int i = 0; i < nx; i++ ){
    double x = rT_->lower() + dd * i;
    g->SetPoint( i, x, (*rho_)( x ) );
  }
  
  g->SetLineColor( kRed );
  g->SetLineWidth( 2 );
  g->Draw("LA");
  g->GetXaxis()->SetTitle( "r [nm]" );
  g->GetYaxis()->SetTitle( "#rho(r)" );
  
  double xmax = g->GetXaxis()->GetXmax();
  double xmin = g->GetXaxis()->GetXmin();
  double dx   = 0.1 * ( xmax - xmin );
  double ymax = g->GetYaxis()->GetXmax();
  double ymin = g->GetYaxis()->GetXmin();
  double dy   = 0.1 * ( ymax - ymin );
  
  line_->SetLineWidth( 1 );
  line_->SetLineStyle( 1 );
  
  line_->SetLineColor( kBlue );
  line_->DrawLine( rho_->mean, ymin, rho_->mean, ymax );
  
  line_->SetLineStyle( 2 ); 
  line_->SetLineColor( kBlue + 2 );
  line_->DrawLine( rho_->mean + rho_->asym * rho_->sigma,
		   ymin,
		   rho_->mean + rho_->asym * rho_->sigma,
		   ymax );
  
  line_->SetLineColor( kBlue - 2 );
  line_->SetLineStyle( 3 );
  line_->DrawLine( rho_->mean - rho_->sigma, ymin,
		   rho_->mean - rho_->sigma, ymax );
  
  double txx  = xmin + 1.0 * dx;
  latex_->DrawLatex( txx, 8.5 * dy + ymin, rho_->text().c_str() );
  
}

void MyApplication::drawI( ESR *esr ){
  shared_ptr<TGraph> gESR = esr->GetGraphInteg();
  // find maximum point
  if( this->toffset() == 0.0 ){
    int iMax = 0; double ymax = 0.0; double ymax_x = 0; 
    for( int i = 0; i < gESR->GetN(); i++ ){
      double x, y;
      gESR->GetPoint( i, x, y );
      if( y > ymax ){ iMax = i; ymax = y ; ymax_x = x; }
    }
    this->toffset( ymax_x );
    I0_ = ymax;
  }
  if( gESR_ ) { delete gESR_; }
  gESR_ = dynamic_cast< TGraph* >( gESR->Clone() );
  if( gESR_ ){
    gESR_->SetLineColor( kGreen + 2 );
  }
  this->drawI();
}

void MyApplication::drawI(){
    
  if( c_ ) c_->cd( 2 );

  double tmin = tRange_[ 0 ] + k_->offset();
  double tmax = tRange_[ 1 ] + k_->offset();

  TGraph *g = new TGraph;
  
  // calculate value of the transfered function, g(t), at various t
  double corr = 1.0 / (*rT_)( k_->offset() );
  for( double t = tmin; t < tmax; t += tstep_ ){
    //    double v = I0_ * corr * (*rT_)( t );
    double v = (*rT_)( t );
    int i = g->GetN();
    g->Set( i + 1 );
    g->SetPoint( i, t, v );
  }
  
  g->SetLineColor( kRed );
  g->SetLineWidth( 2 );
  g->Draw("LA");
  g->GetXaxis()->SetTitle( "t [mT]" );
  
  ostringstream ostr;
  ostr << "I(t) = #int^{"
       << rT_->upper()
       << "}_{"
       << rT_->lower()
       << "} #tilde{K}(r,t) #tilde{#rho}(r) dr";
  
  g->GetYaxis()->SetTitle( ostr.str().c_str() );
  
  double xmax = g->GetXaxis()->GetXmax();
  double xmin = g->GetXaxis()->GetXmin();
  double dx   = 0.1 * ( xmax - xmin );
  double ymax = g->GetYaxis()->GetXmax();
  double ymin = g->GetYaxis()->GetXmin();
  double dy   = 0.1 * ( ymax - ymin );

  double dhcen = this->hDcenter();
  line_->SetLineWidth( 1 );
  line_->SetLineStyle( 1 );
  line_->SetLineColor( kBlue );
  line_->DrawLine( dhcen, ymin, dhcen, ymax );
  line_->DrawLine( 2.0 * k_->offset() - dhcen, ymin,
		   -dhcen + 2.0*k_->offset(), ymax );
  
  double dhmin = this->hDlower();
  line_->SetLineStyle( 2 );
  line_->SetLineColor( kBlue + 2 );
  line_->DrawLine( dhmin, ymin, dhmin, ymax );
  line_->DrawLine( 2.0 * k_->offset() - dhmin, ymin,
		   2.0 * k_->offset() - dhmin, ymax );
  
  double dhmax = this->hDupper();
  line_->SetLineStyle( 3 );
  line_->SetLineColor( kBlue - 2 );
  line_->DrawLine( dhmax, ymin, dhmax, ymax );
  line_->DrawLine( 2.0 * k_->offset() - dhmax, ymin,
		   2.0 * k_->offset() - dhmax, ymax );
  
  double txx  = xmin + 0.6 * dx;
  latex_->DrawLatex( txx, 9.0 * dy + ymin, rho_->text().c_str() );
  latex_->DrawLatex( txx, 7.8 * dy + ymin, "#tilde{#rho}(r)=#frac{#pi r^{2}}{3|A(r)|}#rho(r)" );
  latex_->DrawLatex( txx, 6.6 * dy + ymin, k_->text().c_str() );

  if( gESR_ ) { gESR_->Draw( "l" ); }
  
}

void MyApplication::drawI( const double& tmin, const double& tmax ){
  this->tRange( tmin - k_->offset(), tmax - k_->offset() );
  this->drawI();
}



void MyApplication::tRange( const double& min, const double& max ){
  tRange_[ 0 ] = min;
  tRange_[ 1 ] = max;
  tstep_ = ( tRange_[ 1 ] - tRange_[ 0 ] ) / nT_;
}

void MyApplication::showArguments( TGraph *g ){
  double xmax = g->GetXaxis()->GetXmax();
  double xmin = g->GetXaxis()->GetXmin();
  double dx   = 0.1 * ( xmax - xmin );
  double ymax = g->GetYaxis()->GetXmax();
  double ymin = g->GetYaxis()->GetXmin();
  double dy   = 0.1 * ( ymax - ymin );
  double dtx = xmax - xmin;
  double dty = ymax - ymin;
  double tx = 0.7 * dtx + xmin;
  latex_->DrawLatex( 0.6 * dtx + xmin, 0.90 * dty + ymin, "Arguments:" );
  this->drawArgument( tx, 0.82 * dty + ymin, "nleg" );
  this->drawArgument( tx, 0.74 * dty + ymin, "precision" );
  this->drawArgument( tx, 0.66 * dty + ymin, "nGrid" );
  this->drawArgument( tx, 0.58 * dty + ymin, "mean" );
  this->drawArgument( tx, 0.50 * dty + ymin, "sigma" );
  this->drawArgument( tx, 0.42 * dty + ymin, "asym" );
}

void MyApplication::drawArgument( const double& x, const double& y,
				  const std::string& opt ){
  Utility::Arguments &args = Utility::Arguments::ref();
  ostringstream ostr;
  ostr << "--" << opt;
  if( args.hasOpt( opt ) ){
    ostr << "=" << args.get( opt, "" );
  } else {
    ostr << "[= default value]";
  }
  latex_->DrawLatex( x,  y, ostr.str().c_str() );
}

double MyApplication::hDlower(){
  return k_->offset() + k_->localField( rho_->upper() );
}

double MyApplication::hDcenter(){
  return k_->offset() + k_->localField( rho_->mean );
}

double MyApplication::hDupper(){
  return k_->offset() + k_->localField( rho_->lower() );
}

double MyApplication::toffset() { return k_->offset(); }

void MyApplication::toffset( const double& value ) {
  k_->offset( value );
  this->update();
}

void MyApplication::nLeg( const int& n1, const int& n2 ){
  rT_->nLeg( n1, n2 );
}

void MyApplication::nGrid( const int& n ){
  rT_->nGrid( n );
}

void MyApplication::precision( const double& p ){
  rT_->precision( p );
}

void MyApplication::amplitude( const double& v ){
  rho_->A = v;
}

void MyApplication::mean( const double& v ){
  rho_->mean = v;
  this->update();
}

void MyApplication::sigma( const double& v ){
  rho_->sigma = v;
  this->update();
}

void MyApplication::asym( const double& v ){
  rho_->asym = v;
  this->update();
}

double MyApplication::amplitude(){
  return rho_->A;
}

double MyApplication::mean(){
  return rho_->mean;
}

double MyApplication::sigma(){
  return rho_->sigma;
}

double MyApplication::asym(){
  return rho_->asym;
}

void MyApplication::update(){
  
  rT_->upper( rho_->mean + 4.0 * rho_->sigma * rho_->asym );
  rT_->lower( rho_->mean - 4.0 * rho_->sigma );
  if( rT_->lower() < 0.0 ) rT_->lower( 0.0 );
  
  double trange = 1.5 * fabs( this->hDupper() - k_->offset() );
  tRange_[ 0 ] = -trange;
  tRange_[ 1 ] =  trange;

  tstep_ = ( tRange_[ 1 ] - tRange_[ 0 ] ) / nT_;

}

void MyApplication::draw(){
  this->canvas();
  this->drawRho();
  this->drawI();
}

void MyApplication::draw( ESR *esr ){
  this->canvas();
  this->drawRho();
  this->drawI( esr );
}

void MyApplication::canvas(){
  if( c_ == NULL ) {
    c_ = new TCanvas( "myCanvas", "Dipole-Dipole Interaction", 794, 1123 );
    c_->Divide( 1, 2 );
  }
}

LineShape* MyApplication::lineShapeObj(){
  LineShape* lS = new LineShape;
  lS->rT_ = rT_;
  lS->rho_ = rho_;
  return lS;
}

double MyApplication::evalI( const double& t ){
  return (*rT_)( t );
}

ClassImp( MyApplication )
