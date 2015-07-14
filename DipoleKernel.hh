#ifndef _DipoleKernel_hh_
#define _DipoleKernel_hh_

#include <Tranform/RTransform.hh>
#include <string>
#include <vector>
/*
  Definition of the tarnsformation kernel: K(x,t)
  
  In order to create a kernel, user should inherit
  Transform::RTransform::Kernel and define own child class.
  In the child class, user has to overwrite the member function
  double Kernel::eval( const double& x )
  with which user has to calculate the kernel function numerically
  with the given argument of x, and t by obtained with Kernel::t()
  method.

  Details of Kernel functional form can be found in Iwata-sensei's
  Note
https://drive.google.com/a/quark.kj.yamagata-u.ac.jp/file/d/0BwXqnrBuS6yDVHJqV3Zuc1hOVm8/view?usp=sharing

  r: in nm
  t: in mT

*/
class DipoleKernel : public Transform::RTransform::Kernel {
public:

  DipoleKernel();            // default constructor

  virtual ~DipoleKernel();   // destructor
  
  //  f(x) = pow( (x+1)/3, -0.5 )
  double finv( const double& x );

  // f(x) + f(-x)
  double ftilde( const double& x );
  
  /*!
    Unit of the given r is nm and unit of return value is mT.
    Constant term, -1.395, corresponds,
    {\mu_0 \over 4\pi}{ 3\mu_e \over 2 } = -1.395E-30 T/m^3 = -1.395 mT/(nm)^3 
  */
  double localField( const double& r );
  
  // actual definition of the kernel functional form.
  // r in nm
  // t() in mT
  virtual double eval( const double& r );

  std::string text();
  
  void offset( const double& v );
  double offset(); // return mean of offset
  
private:
  std::vector< double > lines_;        // ESR lines
  
  
};

#endif // _DipoleKernel_hh_
