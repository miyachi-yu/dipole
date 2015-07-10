#ifndef _ESRData_hh_
#define _ESRData_hh_

#include <string>
#include <vector>

class TH1;

class ESRData {
public:
  
  ESRData( );                            // default constructor
  ESRData( const std::string& path );    // constructor with data file path
  ESRData( const ESRData& data );        // a copy constructor
  virtual ~ESRData();                    // destructor

  ESRData& operator=( const ESRData& data ); // assignment operator
  
  bool ready(); // return true if histograms are ready

  double xmin() const ;                  // get minimum value of g-factor
  double xmax() const ;                  // get maximum value of g-factor
  double bToG( const double& b ) const ; // convert mT to g-factor
  double iToG( const int& i ) const ;    // convert index to g-factor
  double iToB( const int& i ) const ;    // convert index to g-factor
  
  double amplitude() const {             // get amplitude
    return amplitude_fine_ * amplitude_coarse_;
  }
  
  std::string name() const { return name_; }   // get file name
  double frequency() const { return uwFreq_; } // get u-wave frequency 
  double uwPower() const { return uwPower_; }  // get u-wave power
  
  // set parameters
  void name( const std::string& v ) { name_ = v; }
  void ampCoarse( const double& v ) { amplitude_coarse_ = v; }
  void ampFine(   const double& v ) { amplitude_fine_ = v; }
  void frequency( const double& v ) { uwFreq_ = v; }
  void uwPower( const double& v ) { uwPower_ = v; }
  
  void load( const std::string& path );
  
  static std::vector< double > markerArea( const int& i ); // get marker area ( 1 or 2 )
  
  double markerSize( const double& xlow  = 2.075, 
		     const double& xhigh = 2.098 ); // calculate Mn strength
  
  double dx() const ; // get width of bin
  

private:
  
  std::string name_;
  std::string path_;

  int length_;
  int counter_;
  int dataType_;
  std::string dataKey_;
  
  double x_min_;
  double x_range_;

  double uwFreq_;
  double uwPower_;
  
  double amplitude_fine_;
  double amplitude_coarse_;

  static const std::string commentTag_;
  
  void load( std::istream& is );
  void parseDataLine( const std::string& key, const std::string& data );
  std::string getDataKey();
  TH1* integral( TH1* h1 );
  
  void deleteObjs();
  
public:

  TH1* realPart_;          // real part of ESR spectrum
  TH1* imagPart_;          // imaginary part of ESR spectrum
  
  TH1* integral_;          // integral of the real part histogram
  TH1* integral2_;         // double integral of the real part histogram
  
  TH1* real_mT_;
  TH1* real_int_mT_;
  
  std::vector< double > Mn_; // Mn Marker size (2)
  
};

#endif // _ESRData_hh_
