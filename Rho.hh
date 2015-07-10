#ifndef _Rho_hh_
#define _Rho_hh_
#include <Tranform/RealFunction.hh>
#include <iomanip>
#include <string>
/*
  Definition of the funciton which will be transformed with
  the tranform kernel given above. 
  As a example, the gauss distribution is implemented here.
*/
class Rho : public Transform::RealFunction {
public:
  Rho();
  virtual ~Rho();
  virtual double operator()( const double& x );
  std::string text();
  
  double A;
  double mean;
  double sigma;

  friend std::ostream& operator<<( std::ostream& os, const Rho& rho );
  
};

#endif // _Rho_hh_
