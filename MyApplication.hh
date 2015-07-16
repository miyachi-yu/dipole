#ifndef _MyApplication_hh_
#define _MyApplication_hh_

#include <TRint.h>
#include <vector>

class TLine;
class TLatex;
class TGraph;
class TCanvas;

class DipoleKernel;
class AGaus;
class LineShape;

class ESR;

namespace Transform {
  class RTransform;
  class RealFunction;
}

class MyApplication : public TRint {
public:

  MyApplication( int argc, char* argv[] );
  virtual ~MyApplication();
  
  static MyApplication* instance() { return singleton_; }

  void draw();
  void draw( ESR* esr );
  void drawRho();

  void drawI();
  void drawI( ESR* esr );
  void drawI( const double& tmin, const double& tmax );

  
  std::vector< double >& tRange() { return tRange_; }
  std::vector< double >  tRange() const { return tRange_; }
  void tRange( const double& min, const double& max );
  void amplitude( const double& v );
  void mean( const double& v );
  void sigma( const double& v );
  void asym( const double& v );

  double amplitude();
  double mean();
  double sigma();
  double asym();
  
  double toffset();
  void toffset( const double& value );

  void showArguments( TGraph *g );
  
  void drawArgument( const double& x, const double& y,
		     const std::string& opt );

  double hDlower();
  double hDcenter();
  double hDupper();

  void nLeg( const int& n1, const int& n2 );
  void nGrid( const int& n );
  void precision( const double& p );

  AGaus* rho() { return rho_; } 
  DipoleKernel* kernel(){ return k_; }
  
  LineShape* lineShapeObj();
  
  double evalI( const double& t );
  
private:
  static MyApplication* singleton_;
  
  DipoleKernel* k_;
  AGaus* rho_;
  Transform::RTransform* rT_;

  TLine *line_;
  TLatex *latex_;
  TCanvas *c_;
  
  std::vector< double > tRange_;
  double tstep_;
  int nT_;
  
  double I0_;
  TGraph *gESR_;
  
  void update();
  void canvas();
  
  ClassDef( MyApplication, 1 );
};

#endif //_MyApplication_hh_
