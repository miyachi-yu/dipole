#ifndef __ESRLine_hh__
#define __ESRLine_hh__

#include "ESR.hh"

class ESRLine : public ESR {
public:
  ESRLine( const std::string& name, const int& rfact = 1 );
  virtual ~ESRLine();

  double find( const double& min, const double& max );
  
private:

  ClassDef( ESRLine, 1.0 );
};


#endif // __ESRLine_hh__
