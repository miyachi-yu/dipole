#ifndef __Fitter_hh__
#define __Fitter_hh__

#include <TMinuit.h>

class MyApplication;
class TGraph;

class Fitter : public TMinuit {
public:
  Fitter();
  virtual ~Fitter();
  virtual Int_t Eval( Int_t npar, Double_t* grad,
		      Double_t& fval, Double_t* par, Int_t flag );
  
  void fit( TGraph* g );
  void fit( TGraph* g, const double& min, const double& max );
  
private:
  MyApplication *app_;
  TGraph *g_;
  
  
  ClassDef( Fitter, 1.0 );
};


#endif //__Fitter_hh__
