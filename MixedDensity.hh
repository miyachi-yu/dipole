#ifndef _MixedDensity_hh_
#define _MixedDensity_hh_

#include <Tranform/RealFunction.hh>
#include <TObject.h>
#include <vector>

class MixedDensity : public Transform::RealFunction,
		     public TObject,
		     std::vector< Transform::RealFunction* >
{
public:
  MixedDensity();
  virtual ~MixedDensity();
  virtual double operator()( const double& x );
private:

  ClassDef( MixedDensity, 1.0 );
};

#endif // _MixedDensity_hh_
