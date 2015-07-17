#include "ESRLine.hh"
#include <TGraph.h>
#include <TFitResult.h>
#include <TFitResultPtr.h>
ESRLine::ESRLine( const std::string& name, const int& rfact ) :
  ESR( name, rfact )
{
}

ESRLine::~ESRLine(){
}

double ESRLine::find( const double& min, const double& max ){
  TFitResultPtr ptr = graph_->Fit( "pol1", "QS+", "", min, max );
  return - ptr.Get()->Value( 0 ) / ptr.Get()->Value( 1 );
}

ClassImp( ESRLine );
