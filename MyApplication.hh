#ifndef _MyApplication_hh_
#define _MyApplication_hh_
/*
  Main Application Class for ESR analysis

  In the main program, user_program.cc, this class object will be
  created. After issuing MyApplication::Run() methods, program enters
  interactive mode controlled with the CInt interface.

  Through the CInt interface, one can access directly the MyApplication
  instance, calling (MyApplication*) MyApplication::instance() method.

 */
#include <TRint.h>
#include <vector>

class TLine;
class TLatex;
class TGraph;
class TCanvas;

class DipoleKernel;
class Density;
class LineShape;

class ESR;

namespace Transform {
  class RTransform;
  class RealFunction;
}

/*
  Definition of MyApplication class
 */
class MyApplication : public TRint {
public:

  MyApplication( int argc, char* argv[] );  // constructor
  virtual ~MyApplication();                 // destractor

  // get a MyApplication pointer
  static MyApplication* instance() { return singleton_; }
  
  // draw two panel graph: density distribution at top,
  // intensity distribution at bottom
  void draw(); 

  // draw the two panel graph
  // with the given ESR raw data on the intensity disribution.
  void draw( ESR* esr );

  // draw the density distribution only
  void drawRho();

  // draw the intensity distribution only
  void drawI();

  // draw the intensity distribution with the given ESR data
  void drawI( ESR* esr );

  // draw the intensity distribution in the given range
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

  Density* density() { return rho_; } 
  //  Density* rho() { return rho_; }       // will be merged to density method
  DipoleKernel* kernel(){ return k_; }
  
  LineShape* lineShapeObj();
  
  double evalI( const double& t );

  void update();
private:
  static MyApplication* singleton_;
  
  DipoleKernel* k_;
  Density* rho_;
  Transform::RTransform* rT_;

  TLine *line_;
  TLatex *latex_;
  TCanvas *c_;
  
  std::vector< double > tRange_;
  double tstep_;
  int nT_;
  
  double I0_;
  TGraph *gESR_;
  
  void canvas();
  
  ClassDef( MyApplication, 1 );
};

#endif //_MyApplication_hh_
