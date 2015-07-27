#ifndef _Density_hh_
#define _Density_hh_

#include <Tranform/RealFunction.hh>
#include <TObject.h>
#include <string>

class Density : public Transform::RealFunction,
		public TObject {
public:

  Density();
  virtual ~Density();

  virtual double upper() const = 0;
  virtual double lower() const = 0;
  
  virtual void amplitude( const double& v ) = 0;
  virtual void mean( const double& v ) = 0;
  virtual void sigma( const double& v ) = 0;
  virtual void asym( const double& v ) = 0;
  
  virtual double amplitude( ) const = 0;
  virtual double mean( ) const = 0;
  virtual double sigma( ) const = 0;
  virtual double asym( ) const = 0;
  
  virtual double asigma( const bool& plus ) const = 0;

  virtual std::string text() const = 0;
  
  ClassDef( Density, 1.0 );
};


#endif // _Density_hh_
