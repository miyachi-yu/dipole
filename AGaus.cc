#include "AGaus.hh"
#include <iostream>
#include <sstream>
#include <cmath>

using namespace std;

AGaus::AGaus(): Density(),
		a_( 10.0 ), mean_( 10.0 ), sigmap_( 4.0 ), sigmam_( 4.0 ) {
}

AGaus::~AGaus() {
}

double AGaus::operator()( const double& x ){
  const double a = 0.5 * sqrt( 2.0 * M_PI );
  double area = a * ( sigmap_  + sigmam_ );
  return
    a_ *
    exp( - 0.5 * pow( ( x - mean_ ) / ( x > mean_ ? sigmap_ : sigmam_ ),
		      2.0 ) )
    / area;
}

string AGaus::text() const {
  ostringstream ostr;
  ostr << "#rho(r) = #frac{" << a_ << "}{area}"
       << " #font[02]{exp}#left( -#frac{1}{2} #left(#frac{r-"<< mean_
       <<"}{+"<< sigmap_ <<"/-"<< sigmam_ <<"}#right)^{2}#right)";
  return ostr.str();
}

double AGaus::upper() const {
  return mean_ + 3.0 * sigmap_;
}

double AGaus::lower() const {
  double v = mean_ - 3.0 * sigmam_;
  return ( v > 0.0 ? v : 0.0 );
}

void AGaus::amplitude( const double& v ){ a_ = fabs(v); }
void AGaus::mean( const double& v ){ mean_ = fabs(v); }
void AGaus::sigma( const double& v ){ sigmap_ = sigmam_ = fabs(v); }
void AGaus::asym( const double& v ){ sigmap_ = fabs( v )* sigmam_; }

double AGaus::amplitude( ) const { return a_; }
double AGaus::mean( ) const { return mean_; }
double AGaus::sigma( ) const { return 0.5 * ( sigmap_ + sigmam_ ); }
double AGaus::asigma( const bool& plus ) const {  return plus ? sigmap_ : sigmam_; }
void AGaus::asigma( const bool& plus, const double& v ) {
  if( plus ) sigmap_ = fabs( v );
  else sigmam_ = fabs( v );
}
double AGaus::asym( ) const { return sigmap_ / sigmam_; }

ostream& operator<<( ostream& os, const AGaus& rho ){
  os << "A:" << setw(10) << rho.amplitude()
     << ", mean: " << setw(10) << rho.mean()
     << ", sigma: " << setw(10) << rho.sigma()
     << ", asymmetry: " << setw(10) << rho.asym();
  return os;
}

ClassImp( AGaus );
