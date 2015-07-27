#ifndef _LineShape_hh_
#define _LineShape_hh_

#include <TObject.h>

namespace Transform {
  class RTransform;
}
//class DipoleKernel;
class Density;

class LineShape : public TObject {
public:  
  double operator()( double *x, double *p );
  Transform::RTransform *rT_;
  //  DipoleKernel* k_;
  Density *rho_;
  double base_;
  ClassDef( LineShape, 1.0 );
};


#endif // _LineShape_hh_
