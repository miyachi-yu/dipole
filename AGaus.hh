#ifndef _AGaus_hh_
#define _AGaus_hh_
#include "Density.hh"

#include <iomanip>
#include <string>

/*
  Definition of the funciton which will be transformed with
  the tranform kernel given above. 
  As a example, the gauss distribution is implemented here.
*/
class AGaus : public Density {
public:
  AGaus();
  virtual ~AGaus();
  virtual double operator()( const double& x );

  virtual double upper() const ;
  virtual double lower() const ;

  virtual void amplitude( const double& v );
  virtual void mean( const double& v );
  virtual void sigma( const double& v );
  virtual void asym( const double& v );
  
  virtual double amplitude( ) const;
  virtual double mean( ) const;
  virtual double sigma( ) const;
  virtual double asym( ) const;
  
  virtual double asigma( const bool& plus ) const ;
  virtual void asigma( const bool& plus, const double& v );
  
  virtual std::string text() const;

  friend std::ostream& operator<<( std::ostream& os, const AGaus& rho );

private:
  
  double a_;
  double mean_;
  double sigmap_;
  double sigmam_;
  
  ClassDef( AGaus, 1.0 );
};

#endif // _Rho_hh_
