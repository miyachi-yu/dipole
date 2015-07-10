#include "ESRDATA.hh"
#include <iomanip>
#include <algorithm>
#include "TArrow.h"


ESRDATA::ESRDATA( string sdataname ){
  filename_ = sdataname;
  ifstream ifs( sdataname.c_str() );
  if( ifs.fail() ){
    cout << "ERROR" << endl;
    exit( -1 );
  }

  int itemp;
  double dtemp;
  string stemp;

  Skip( ifs , 3 , 14); // get "data length"
  ifs >> itemp;
  data_length_ = itemp;

  Skip( ifs, 8, 14 );// get "x-view min"
  ifs >> dtemp;
  xmin_ = dtemp;

  Skip( ifs, 1, 14 );// get "x-view max"
  ifs >> dtemp;
  xmax_ = dtemp;

  Skip( ifs, 1, 14 );// get "y-view min"
  ifs >> dtemp;
  ymin_ = dtemp;

  Skip( ifs, 1, 14 );// get "y-view max"
  ifs >> dtemp;
  ymax_ = dtemp;

  Skip( ifs ,8, 14 );// get "date_"
  ifs >> stemp;
  date_ = stemp;

  Skip( ifs, 16, 22 );// get "amplitude(fine)" and "amplitude(course)"
  ifs >> dtemp;
  double fine = dtemp;
  Skip( ifs, 1, 23 );
  ifs >> itemp;
  int course = itemp;
  gain_ = fine * pow( 10, course );

  Skip( ifs, 40 ,0 );// get "Real part data"
  ExtractData( ifs );

  graph_ = MakeGraph( vxdata_, vydata_ );
  graph_norm_ = MakeGraph( vxdata_, vydata_norm_ );

}

void ESRDATA::DrawArrows()
{
  TArrow* ar = new TArrow();
  ar->SetLineWidth( 3 );
  ar->DrawArrow( 312.25, 0, 312.25, ymax_*ymax_/2, 0.05 , "<");
  ar->DrawArrow( 338.4 , 0, 338.4 , ymax_*ymax_/2, 0.05 , "<");
  //  ar->DrawArrow( 342.25, 0, 342.25, ymax_*ymax_/2, 0.05 , "<"); for room temprature ?
}

void ESRDATA::ExtractData( ifstream& ifs )
{
  double dtemp;
  double dx = (xmax_ - xmin_)/(double)data_length_;

  for( int i=0; i<data_length_; i++)
    {
      ifs >> dtemp;
      vydata_.push_back( dtemp );
      vydata_norm_.push_back( dtemp/gain_ );
      vxdata_.push_back( xmin_ + i*dx );
    }
}

void ESRDATA::GetData( string type, vector < double > &data )
{

  data.reserve( vxdata_.size() );
  if( type == "x" )
    {

      copy( vxdata_.begin(), vxdata_.end(), back_inserter( data ) );
    }
  else if( type == "y")
    {
      copy( vydata_.begin(), vydata_.end(), back_inserter( data ) );
    }
  else if( type == "norm" )
    {
      copy( vydata_norm_.begin(), vydata_norm_.end(), back_inserter( data ) );
    }
  else
    {
      cout << "Please input x, y or norm" << endl;
      exit( -1 );
    }
}

TGraph* ESRDATA::GetGraphInteg()
{

  TGraph* graph = GetGraph();
  double dx = (xmax_ - xmin_ )/static_cast<double>(data_length_);
  double dtempx, dtempy;
  double ybuff = 0.0;
  for( int i=0; i<vxdata_.size(); i++)
    {
      graph->GetPoint( i , dtempx, dtempy );

      ybuff += dtempy * dx;
      vydata_int_.push_back( ybuff );
    }

  return new TGraph( vxdata_.size(), &vxdata_[0], &vydata_int_[0] );
}


TGraph* ESRDATA::GetGraphIntegPart( double from, double to )
{

  if( xmin_ > from  || from > to || to > xmax_ )
    {
      cout << "ESRDATA::GetGraphIntegPart(double , double )" << endl;
      cout << "2 arguments should be ...." << endl;
      cout << "    " << xmin_ << " < 1st(" << from << ") < 2nd(" << to << ") < " << xmax_ << endl;
      cout << "Please crrect! " << endl;
      exit( -1 );
    }

  TGraph* graph = GetGraph();
  vector < double > x, y ;

  double dtempx, dtempy;
  double dx = (xmax_ - xmin_ )/(double)data_length_;
  int istart = (int)( (from - xmin_)/dx );
  int iend = (int)( (to - xmin_)/dx );

  double ybuff = 0.0;
  for( int i=istart; i<iend; i++)
    {
      graph->GetPoint( i , dtempx, dtempy );

      x.push_back( vxdata_[i] );
      ybuff += dtempy;
      y.push_back( ybuff );
    }

  TGraph* integ = new TGraph( x.size(), &x[0], &y[0] );
  return integ;
}

TGraph* ESRDATA:: MakeGraph( vector < double >& x, vector < double >& y )
{

  TGraph* g = new TGraph( x.size(), &x[0], &y[0] );
  g->SetMarkerStyle( 20 );
  return g;
}

void ESRDATA::Skip( ifstream& ifs, int num , const int byte)
{

  if( num < 0 )
    {
      cout << "void ESRDATA::Skip( ifstream, int)\n";
      cout << "  Please input positive number in 2nd argument!\n";
      exit( -1 );
    }

  string stemp;

  for( int i=0; i<num; i++)
    {
      getline( ifs, stemp );
    }

  SkipByte( ifs , byte);
}

void ESRDATA::SkipByte( ifstream& ifs , const int byte)
{

  char cbuff[byte];
  for( int i=0; i<byte; i++)
    {
      ifs.read( (char*) &cbuff[i], 1 );
    }
}

void ESRDATA::PrintRange()
{

  cout << "Data range-------" << endl;
  cout << "x:from " << xmin_ << "\tto\t" << xmax_ << "\t(width: " << xmax_ - xmin_ << ")" << endl;
  cout << "y:from " << ymin_ << "\tto\t" << ymax_ << "\t(width: " << ymax_ - ymin_ << ")" << endl;

}

// ----------------------------------------------------------------------
string  ESRDATA::GetDataName(){ return filename_; };
int     ESRDATA::GetDataLength(){return data_length_;};
string  ESRDATA::GetDate(){ return date_;};
double  ESRDATA::GetGain(){ return gain_;};
TGraph* ESRDATA::GetGraph(){return graph_;};
TGraph* ESRDATA::GetGraphNorm(){ return graph_norm_;};
vector<double> ESRDATA::GetVX() const {return vxdata_;};
vector<double> ESRDATA::GetVY() const {return vydata_;};
vector<double> ESRDATA::GetVYI() const {return vydata_int_;};
