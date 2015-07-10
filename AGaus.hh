#ifndef _AGaus_hh_
#define _AGaus_hh_
#include <Tranform/RealFunction.hh>
#include <iomanip>
#include <string>

#include <TObject.h>


/*
  Definition of the funciton which will be transformed with
  the tranform kernel given above. 
  As a example, the gauss distribution is implemented here.
*/
class AGaus : public TObject, public Transform::RealFunction {
public:
  AGaus();
  virtual ~AGaus();
  virtual double operator()( const double& x );
  std::string text();

  double upper() { return mean + asym * sigma; }
  double lower() { return mean - sigma; }
  
  double A;
  double mean;
  double sigma;
  double asym;
  
  friend std::ostream& operator<<( std::ostream& os, const AGaus& rho );
  ClassDef( AGaus, 1.0 );
};

#endif // _Rho_hh_
