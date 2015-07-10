#include "ESRData.hh"

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>

#include <cstdlib>

#include <TH1.h>
#include <TF1.h>

const std::string ESRData::commentTag_ = "=====";

using namespace std;

ESRData::ESRData( ) : 
  name_( "" ), path_( "" ), 
  length_( 0 ), counter_( 0 ), dataType_( 0 ), dataKey_( "" ),
  x_min_( 0.0 ), x_range_( 0.0 ), uwFreq_( 0.0 ), uwPower_( 0.0 ),
  amplitude_fine_( 1.0 ), amplitude_coarse_( 1.0 ),
  realPart_( NULL ), imagPart_( NULL ), integral_( NULL ), integral2_( NULL ),
  real_mT_( NULL ), real_int_mT_( NULL ),
  Mn_( 2, 0.0 )
{
}

ESRData::ESRData( const ESRData& data ) : 
  name_( data.name_ ), path_( data.path_ ), 
  length_( data.length_ ), counter_( data.counter_ ), 
  dataType_( data.dataType_ ), dataKey_( data.dataKey_ ),
  x_min_( data.x_min_ ), x_range_( data.x_range_ ), 
  uwFreq_( data.uwFreq_ ), uwPower_( uwPower_ ),
  amplitude_fine_( data.amplitude_fine_ ),
  amplitude_coarse_( data.amplitude_coarse_ ),
  realPart_( NULL ), 
  imagPart_( NULL ), 
  integral_( NULL ), 
  integral2_( NULL ),
  real_mT_( NULL ), real_int_mT_( NULL ),
  Mn_( data.Mn_ )
{
  if( data.realPart_ != NULL ) realPart_ = dynamic_cast< TH1* >( data.realPart_->Clone() );
  if( data.imagPart_ != NULL ) imagPart_ = dynamic_cast< TH1* >( data.imagPart_->Clone() );
  if( data.integral_ != NULL ) integral_ = dynamic_cast< TH1* >( data.integral_->Clone() );
  if( data.integral2_ != NULL ) integral2_ = dynamic_cast< TH1* >( data.integral2_->Clone() );
  if( data.real_mT_ != NULL ) real_mT_ = dynamic_cast< TH1* >( data.real_mT_->Clone() );
  if( data.real_int_mT_ != NULL ) real_int_mT_ = dynamic_cast< TH1* >( data.real_int_mT_->Clone() );
}

ESRData::ESRData( const std::string& path ) : 
  name_( "" ), path_( "" ), 
  length_( 0 ), counter_( 0 ), dataType_( 0 ), dataKey_( "" ),
  x_min_( 0.0 ), x_range_( 0.0 ), uwFreq_( 0.0 ), uwPower_( 0.0 ),
  amplitude_fine_( 1.0 ), amplitude_coarse_( 1.0 ),
  realPart_( NULL ), imagPart_( NULL ), integral_( NULL ), integral2_( NULL ),
  real_mT_( NULL ), real_int_mT_( NULL ),
  Mn_( 2, 0.0 )
{
  this->load( path );
}

ESRData::~ESRData(){
  this->deleteObjs();
}

bool ESRData::ready() {
  return realPart_ != NULL && imagPart_ != NULL && integral_ != NULL && integral2_ != NULL ;
}


void ESRData::deleteObjs(){
  
  if( realPart_ != NULL ) delete realPart_;
  if( imagPart_ != NULL ) delete imagPart_;
  if( integral_ != NULL ) delete integral_;
  if( integral2_ != NULL ) delete integral2_;
  if( real_mT_ != NULL ) delete real_mT_;
  if( real_int_mT_ != NULL ) delete real_int_mT_;
  
}


ESRData& ESRData::operator=( const ESRData& data ){
  if( this != &data ){
    
    name_ = data.name_ ;
    path_ = data.path_ ; 
    length_ = data.length_ ;
    counter_ = data.counter_ ; 
    dataType_ = data.dataType_ ;
    dataKey_ = data.dataKey_ ;
    x_min_ = data.x_min_ ;
    x_range_ = data.x_range_ ; 
    uwFreq_ = data.uwFreq_ ;
    uwPower_ = uwPower_ ;
    amplitude_fine_ = data.amplitude_fine_ ;
    amplitude_coarse_ = data.amplitude_coarse_ ;
    
    this->deleteObjs();
    
    realPart_ = ( data.realPart_ ? dynamic_cast< TH1* >( data.realPart_->Clone() ) : NULL );
    imagPart_ = ( data.imagPart_ ? dynamic_cast< TH1* >( data.imagPart_->Clone() ) : NULL );
    integral_ = ( data.integral_ ? dynamic_cast< TH1* >( data.integral_->Clone() ) : NULL );
    integral2_ = ( data.integral2_ ? dynamic_cast< TH1* >( data.integral2_->Clone() ) : NULL );
    
  if( data.real_mT_ != NULL ) real_mT_ = dynamic_cast< TH1* >( data.real_mT_->Clone() );
  if( data.real_int_mT_ != NULL ) real_int_mT_ = dynamic_cast< TH1* >( data.real_int_mT_->Clone() );

  Mn_ = data.Mn_;
    
  }
  return *this;
}

void ESRData::load( const string& path ){
  path_ = path;
  ifstream ifs( path_.c_str() );
  if( ifs ){
    cout << "Load " << path_ << endl;
    this->load( ifs );
  } else {
    cerr << "Failed to open " << path_ << endl;
  }
}

void ESRData::load( std::istream& is ){
  
  string line;                                   // buffer line
  dataType_ = 0;
  dataKey_  = "";
  counter_  = 0;
  
  while( getline( is, line ) ){
    
    if( line.substr( 0, commentTag_.size() ) == commentTag_ ) continue;
    
    int pos = line.find( "= " );
    if( pos != string::npos ){            // header information
      this->parseDataLine( line.substr( 0, pos ),
			   line.substr( pos + 2 ) );
    } else if( dataType_ != 0 ) {         // ESR spectrum data part
      float d = atof( line.c_str() ) / this->amplitude();
      float x = this->iToG( counter_ );
      if( dataType_ == 1 && realPart_ != NULL ) realPart_->Fill( x, d );
      if( dataType_ == 1 && real_mT_ != NULL ) real_mT_->Fill( this->iToB( counter_ ), d );
      if( dataType_ == 2 && imagPart_ != NULL ) imagPart_->Fill( x, d );
      counter_++;
    }
  }
  
  if( realPart_ != NULL )
    integral2_ = this->integral( integral_ = this->integral( realPart_ ) );
  
  if( real_mT_ != NULL ) real_int_mT_ = this->integral( real_mT_ );
  
  for( int im = 0; im < 2; im++ ){
    vector< double > area = this->markerArea( im + 1 );
    Mn_[ im ] = this->markerSize( area[ 0 ], area[ 1 ] );
  }
  
}

void ESRData::parseDataLine( const std::string& tab, 
			     const std::string& data ){
  
  if( tab == string( "length      " ) ){
    length_   = atoi( data.c_str() );
    dataKey_  = this->getDataKey();
    
    realPart_ = new TH1F( string( name_ + ":Re" ).c_str() , 
			  string( name_ + " real part" ).c_str(),
			  length_, this->xmin(), this->xmax() );

    realPart_->GetXaxis()->SetTitle( "g-factor" );
    
    imagPart_ = new TH1F( string( name_ + ":Im" ).c_str(),
			  string( name_ + " imaginary part" ).c_str(), 
			  length_, this->xmin(), this->xmax() );
    
    real_mT_ = new TH1F( string( name_ + ":Re_mT" ).c_str() , 
			 string( name_ + " real part [mT]" ).c_str(),
			  length_, this->x_min_,
			 this->x_min_ + this->x_range_ );
    
    return;
  }
  
  if( tab == string( "file name   " ) ){
    name_ = data.substr( 0, data.size() -1 ); // remove return code
    return;
  } 
  
  if( tab == string( "x-range min " ) ){
    x_min_ = atof( data.c_str() );
    return;
  }
  
  if( tab == string( "x-range     " ) ){
    x_range_ = atof( data.c_str() );
    return;
  }
  
  if( tab == string( "micro frequency   " ) ){
    uwFreq_ = atof( data.substr( 2 ).c_str() );
    return;
  }
  
  if( tab == string( "micro freq. unit  " ) ){
    if( data.substr( 2 ) == "kHz" ) uwFreq_ /= 1000.0; // keep in MHz
    if( data.substr( 2 ) == "GHz" ) uwFreq_ *= 1000.0; // keep in MHz
    return;
  }
  
  if( tab == string( "micro power       " ) ){
    uwPower_ = atof( data.substr( 2 ).c_str() );
    return;
  }
  
  if( tab == string( "micro power unit  " ) ){
    if( data.substr( 2 ) == "W" )  uwPower_ /= 1000.0; // keep in mW
    if( data.substr( 2 ) == "uW" ) uwPower_ *= 1000.0; // keep in mW
    return;
  }
  
  if( tab == string( "amplitude(fine)   " ) ){
    amplitude_fine_ = atof( data.substr( 2 ).c_str() );
    return;
  }

  if( tab == string( "amplitude(coarse) " ) ){
    amplitude_coarse_ = pow( 10.0, atoi( data.substr( 2 ).c_str() ) );
    return;
  }

  
  if( dataKey_ != "" && 
      tab.substr( 0, dataKey_.size() ) == dataKey_ ) { 
    
    if( data.substr( 0, 14 ) == string( "Real part data" ) ) dataType_ = 1;
    if( data.substr( 0, 19 ) == string( "Imaginary part data" ) ) dataType_ = 2;
    counter_ = 0;                                   // counter reset
    return;
  }
}

std::string ESRData::getDataKey() {
  ostringstream dataKey;
  dataKey << "data[0.." << length_ - 1 << "]";
  return dataKey.str();
}

TH1* ESRData::integral( TH1* h1 ){

  if( h1 == NULL ) return NULL;
  
  TH1 *copy = 
    dynamic_cast< TH1* > ( h1->Clone( ( string( h1->GetName() ) + ":Int" ).c_str() ) );
  
  double weight = this->dx();
  
  for( int bin = 0; bin < copy->GetNbinsX(); bin++ ){
    copy->SetBinContent( bin, 
			 weight * copy->GetBinContent( bin ) + 
			 ( bin == 0 ? 0.0 : copy->GetBinContent( bin - 1 ) ) ); 
  }
  copy->SetBinContent( copy->GetNbinsX(), 0.0 );
  
  return copy;
}

double ESRData::iToG( const int& i ) const {
  if( length_ == 0 ) return i;
  double dx     = ( this->xmax() - this->xmin() ) / length_;
  double offset = ( i + 0.5 ) * dx;
  double x      = this->xmin() + offset;
  return x;
}

double ESRData::iToB( const int& i ) const {
  if( length_ == 0 ) return i;
  double dx     = this->x_range_ / length_;
  return this->x_min_ + dx * ( i + 0.5 );
}

// B in mT
double ESRData::bToG( const double& B ) const {
  
  const double mu_B = 5.7883818066E-11;         // MeV T^{-1}
  const double hc   = 197.3269718 * 2.0 * M_PI; // MeV fm
  const double c    = 2.99792458E+8;            // m s^{-1}
  const double muB_c = mu_B * c;
  const double A    =  hc * 1.0E-6 / muB_c;
  
  return ( A * uwFreq_ / B );   
}

double ESRData::xmin() const {
  return this->bToG( x_min_ + x_range_ );
}

double ESRData::xmax() const { 
  return this->bToG( x_min_ );
}

double ESRData::markerSize( const double& xlow, const double& xhigh ){
  if( realPart_ == NULL ) return 0.0;

  if( xlow >= xhigh ) return 0.0 ;

  TF1 f1( "p2", "pol1" );
  realPart_->Fit( &f1, "Q", "", xlow, xhigh );
  double p0 = f1.GetParameter( 0 );
  double p1 = f1.GetParameter( 1 );
  
  double d = - ( p0 * xlow + p1 * pow( xlow, 2 ) / 2.0  );
  
  double Sbk = 
    (  p0 * pow( xhigh, 2 ) / 2.0 + p1 * pow( xhigh, 3 ) / 6.0 + d * xhigh ) 
    - 
    (  p0 * pow( xlow, 2 ) /  2.0 + p1 * pow( xlow, 3 ) /  6.0 + d * xlow );
  
  double StotUp = integral2_->GetBinContent( integral2_->FindBin( xhigh ) );
  double StotDw = integral2_->GetBinContent( integral2_->FindBin( xlow  ) );
  double Stot = StotUp - StotDw;
  
  return Stot - Sbk;
}


double ESRData::dx() const {
  return length_ > 0.0 ? ( this->xmax() - this->xmin() ) / length_ : 0.0 ;
}
  

vector< double > ESRData::markerArea( const int& i ){
  vector< double > area( 2, 0.0 );
  switch( i ){
  case 1: area[ 0 ] = 1.917; area[ 1 ] = 1.934; break;
  case 2: area[ 0 ] = 2.077; area[ 1 ] = 2.096; break;
  }
  return area;
}
