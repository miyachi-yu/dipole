#ifndef ESRDATA_hh
#define ESRDATA_hh

#include <iostream>
#include <vector>
#include "TGraph.h"

using namespace std;

class ESRDATA{
private:

  string filename_;
  int    data_length_;
  double xmin_;
  double xmax_;
  double ymin_;
  double ymax_;
  string date_;
  double gain_; // amplitude(fine) * 10^(amplitude(course) )
  vector < double > vxdata_;
  vector < double > vydata_;
  vector < double > vydata_norm_;
  vector < double > vydata_int_;
  TGraph* graph_;
  TGraph* graph_norm_;
  //  TGraph* integ_;
  //  TGraph* integ_part_;
public:

  ESRDATA( string sdataname );
  void    DrawArrows();
  void    ExtractData(ifstream& ifs );
  string  GetDataName();
  int     GetDataLength();
  void    GetData( string type, vector < double > &data );
  string  GetDate();
  double  GetGain();
  TGraph* GetGraph();
  TGraph* GetGraphNorm();
  TGraph* GetGraphInteg();
  TGraph* GetGraphIntegPart( double from, double to );
  TGraph* MakeGraph( vector < double >& x, vector < double >& y );
  void    Skip( ifstream& ifs, int num );
  void    Skip( ifstream& ifs, int num, const int byte);
  void    SkipByte( ifstream& ifs , const int byte);
  void    PrintRange();

  vector<double> GetVX() const;
  vector<double> GetVY() const;
  vector<double> GetVYI() const; // integrated Y data array.
};
#endif
