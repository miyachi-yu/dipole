#ifndef ESRHEADER_HH
#define ESRHEADER_HH


#include "ESRHeaderElement.hh"
#include <iostream>
#include <fstream>
#include <memory>
#include <map>

#include "TObject.h"

// (rather) read-only class. no setter.
class ESRHeader : public TObject
{
  std::shared_ptr<ESRHeaderDH> header_dh_;
  std::shared_ptr<ESRHeaderGP> header_gp_;
  std::shared_ptr<ESRHeaderSP> header_sp_;
  std::shared_ptr<ESRHeaderAP> header_ap_;
  std::shared_ptr<ESRHeaderDT> header_dt_;

public:
  ESRHeader();
  ESRHeader(const std::map<std::string, std::string>&); // map
  ESRHeader(const ESRHeader&) = default;
  ~ESRHeader();

  static int CheckFileFormatType(std::ifstream&);

  // getter
  int GetDataLength() const;
  std::string GetDate() const;
  std::pair<double, double> GetXrange() const;
  std::pair<double, double> GetYrange() const;
  std::pair<double, double> GetAmplitude(int type = 1) const;

  std::shared_ptr<ESRHeaderDH> GetDataHead() const;
  std::shared_ptr<ESRHeaderGP> GetGeneralParameter() const;
  std::shared_ptr<ESRHeaderSP> GetSpectometerParameter() const;
  std::shared_ptr<ESRHeaderAP> GetAcquisitionParameter() const;
  std::shared_ptr<ESRHeaderDT> GetEsrData() const;

  // setter
  void SetXrange(std::pair<double, double>);
  void SetAmplitude(std::pair<double, double>, int type = 1);

  // print function
  virtual void Print(Option_t* options = "") const final;


  ClassDef(ESRHeader, 1);
};

#endif
