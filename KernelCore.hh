#ifndef _KernelCore_hh_
#define _KernelCore_hh_

#include <string>
#include <TObject.h>

class KernelCore : public TObject {
public:
  
  KernelCore();            // default constructor

  virtual ~KernelCore();   // destructor
  
  //  f(x) = pow( (x+1)/3, -0.5 )
  double f( const double& x );
  
  // f(x) + f(-x)
  double ftilde( const double& x );

  std::string text();

  double operator()( const double& r, const double& t );
  
private:
  ClassDef( KernelCore, 1.0 );
};

#endif // _KernelCore_hh_
