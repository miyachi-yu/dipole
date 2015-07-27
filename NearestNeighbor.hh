#ifndef __NearestNeighbor_hh__
#define __NearestNeighbor_hh__

#include "Density.hh"

/*
  Nearest Distance Distribution in 3D:
  4 pi r^2 rho exp( - (4/3) pi r^3 rho )
 */
class NearestNeighbor : public Density {
public:

  NearestNeighbor();
  virtual ~NearestNeighbor();
  
  virtual double operator()( const double& x );

  virtual double upper() const ;
  virtual double lower() const ;
  
  virtual void amplitude( const double& v );
  virtual void mean( const double& v );
  virtual void sigma( const double& v ){ return; }
  virtual void asym( const double& v ){ return; }

  virtual double amplitude() const ;
  virtual double mean() const ;
  virtual double sigma() const ;
  virtual double asym() const { return 1.0; }

  virtual double asigma( const bool& plus ) const ;

  virtual std::string text() const ;
  
private:
  double a_;
  double rho_;

  ClassDef( NearestNeighbor, 1.0 );
};


#endif // __NearestNeighbor_hh__
