/*
  Sample code for RealFunction Transformation:

  This code show how to transform a real function, f(x)
  using tranformation kernel, K(x,t),
  to a real function, g(t).
  g(t) = int dx K(x,t) f(x)

 */
#include "MyKernel.hh"
#include "DipoleKernel.hh"
#include "Rho.hh"
#include "AGaus.hh"
#include "ESRData.hh"
#include "MyApplication.hh"

#include <Tranform/RTransform.hh>
#include <Utility/Arguments.hh>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cstdlib>

#include <TROOT.h>
#include <TApplication.h>
#include <TCanvas.h>
#include <TGraph.h>
#include <TAxis.h>
#include <TLatex.h>
#include <TLine.h>
#include <TH1.h>
#include <TLegend.h>

using namespace std;

void usage(){
  Utility::Arguments &args = Utility::Arguments::ref();
  
  args.log() << "-------------------------------------------------" << endl;
  args.log() << "Base distribution configuration" << endl;
  args.usage( "sigma", 4.0, "sigma of the base distribution (nm)" );
  args.usage( "mean", 10.0, "mean of the base distribution (nm)" );
  args.log() << "\nRange of t value" << endl;
  args.usage( "tmin", -5.0, "lower edge of t (mT)" );
  args.usage( "tmax",  5.0, "upper edge of t (mT)" );
  args.usage( "tstep",  0.5, "step in t (mT)" );
  args.log() << "\nIntegration setting" << endl;
  args.usage( "xmin", 0.0, "lower edge of x (nm)" );
  args.usage( "xmax", 100.0, "upper edge of x (nm)" );
  args.usage( "nleg", "4,6", "lower and upper limit of LegQuadrature" );
  args.usage( "nGrid", 4, "number of segment for grid integration" );
  args.usage( "precision", 0.01, "required precision for the grid integration" );
  args.log() << "-------------------------------------------------" << endl; 
  
  exit( 0 );
}

int main( int argc, char* argv[] ){
  
  // In order to handle argument list given in command line
  Utility::Arguments &args = Utility::Arguments::ref( argc, argv );

  // If help or ? is found in the argument list,
  // show usage and exit this program
  if( args.hasOpt( "?" ) || args.hasOpt( "help" ) ) usage();

  MyApplication myApp( argc, argv );
  myApp.Run();
  
  return 0;
}
