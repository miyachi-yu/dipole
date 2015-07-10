#ifndef ESRHEADERELEMENT_HH
#define ESRHEADERELEMENT_HH

#include <iostream>
#include <fstream>
#include <memory>
#include <map>

#include "TObject.h"

/*
  add _ at the end of name to represent private member.
 */
// ------------------------------------------------------------
// ------------------------------------------------------------
// ------------------------------------------------------------
class ESRHeaderElement : public TObject
{
protected:
  // helper function
  std::string mysubstr(std::string, size_t);
  std::string mysubstr(std::string, size_t, size_t);
  std::string mystos(const std::map<std::string, std::string>&, std::string);
  int mystoi(const std::map<std::string, std::string>&, std::string);
  int mystoi(const std::string&);
  double mystod(const std::map<std::string, std::string>&, std::string);
  double mystod(const std::string&);

  // virtual method
  virtual void set_val(const std::map<std::string, std::string>&);

public:
  ESRHeaderElement();
  ESRHeaderElement(const std::map<std::string, std::string>&);
  ESRHeaderElement(const ESRHeaderElement&) = default;
  std::map<std::string, std::string> GetParams() const;

  virtual void Print(Option_t* options = "") const;


  ClassDef(ESRHeaderElement, 1);
};


// derived calsses
// ------------------------------------------------------------
// ------------------------------------------------------------
// ------------------------------------------------------------
class ESRHeaderDH : public ESRHeaderElement
{
  std::string file_name_;
  int data_number_;
  int data_length_;
  std::string data_sort_;
  double x_range_min_;
  double x_range_;
  std::string x_unit_;
  std::pair<double, double> minmax_value_;
  std::string y_unit_;
  std::pair<double, double> x_view_;
  std::pair<double, double> y_view_;
  std::string type_;

  virtual void set_val(const std::map<std::string, std::string>&);

public:
  ESRHeaderDH();
  ESRHeaderDH(const std::map<std::string, std::string>&);
  ESRHeaderDH(const ESRHeaderDH&) = default;

  int GetDataLength() const;
  std::pair<double, double> GetXrange() const;
  std::pair<double, double> GetYrange() const;
  std::string GetFileName() const;

  void SetDataLength(int val);
  void SetXrangeMin(double val);
  void SetXrange(double val);

  virtual void Print(Option_t* options = "") const final;

  ClassDef(ESRHeaderDH, 1);
};


// ------------------------------------------------------------
// ------------------------------------------------------------
// ------------------------------------------------------------
class ESRHeaderGP : public ESRHeaderElement
{
  std::string title_;
  std::string sample_name_;
  std::string comment_;
  std::string system_name_;
  std::string version_;
  std::string date_; // string date
  std::string origin_file_;
  std::string prev_file_;

  virtual void set_val(const std::map<std::string, std::string>&);

public:
  ESRHeaderGP();
  ESRHeaderGP(const std::map<std::string, std::string>&);
  ESRHeaderGP(const ESRHeaderGP&) = default;

  std::string GetDate() const;
  std::string GetFileTitle() const;
  std::string GetSampleName() const;
  std::string GetComment() const;

  void SetDate(std::string val);

  virtual void Print(Option_t* options = "") const final;


  ClassDef(ESRHeaderGP, 1);
};


// ------------------------------------------------------------
// ------------------------------------------------------------
// ------------------------------------------------------------
class ESRHeaderSP : public ESRHeaderElement
{
  std::string type_;
  double center_field_;
  int zero_; // zr32768
  int zero2_;
  std::string rcv_mode_;// = ""; // receiver mode
  std::string rcv_mode2_;
  std::string reserved_area_; // RESRVD

  struct Sweep
  {
    std::string control = ""; //swquit
    double time = 0; // unit is second(?)
    std::pair<double, double> width = {0, 0}; // (fine, coarse)
    double mod_freq = 0; // modulation frequency
    std::pair<double, double> phase = {0, 0}; // (fine, coarse);
    std::pair<double, double> phase2 = {0, 0}; // (fine, coarse);
    std::pair<double, double> mod_width = {0, 0}; // (fine, coarse)
    std::pair<double, double> amp1 = {0, 0}; // (fine, coarse)
    std::pair<double, double> amp2 = {0, 0}; // (fine, coarse)
    double tc1 = 0; // time constant1
    double tc2 = 0; // time constant2
  } SW;

  struct MicroWave
  {
    double freq = 0;
    std::string freq_unit = "";
    double power = 0;
    std::string pwr_unit = "";
    int phase = 0;
    int coupling = 0;
    bool is_30db = false;
    bool is_ref= false;
    bool is_gunp = false;
  } MW;

  struct Temperature
  {
    std::string vt_type = ""; // --
    bool temp_control = false;
    double temperature = 300; // RT is assumed to be 300 Kelvin
    std::string tmpr_unit = ""; //
  } TMPR;

  // method
  virtual void set_val(const std::map<std::string, std::string>&);

public:
  ESRHeaderSP();
  ESRHeaderSP(const std::map<std::string, std::string>&);
  ESRHeaderSP(const ESRHeaderSP&) = default;

  std::pair<double, double> GetAmplitude(int type = 1) const; // 1 or 2
  double GetGain() const;
  MicroWave GetMW() const;
  Sweep GetSW() const;
  Temperature GetTMPR() const;

  void SetAmplitude(std::pair<double, double> ,int type = 1);

  virtual void Print(Option_t* options = "") const final;


  ClassDef(ESRHeaderSP, 1);
};


// ------------------------------------------------------------
// ------------------------------------------------------------
// ------------------------------------------------------------
class ESRHeaderAP : public ESRHeaderElement
{
  int accumu_count_;
  int delay_time_;
  int interval_time_;
  int reserved_int_;
  int sampling_time_;
  float reserved_float_; // float... double is prefer...
  std::string accumu_mode_;
  std::string baseline_;
  std::string sampling_mode_;
  std::string signal_trigger_;
  std::string ref_file_name_;
  std::string reserved_char_;

  virtual void  set_val(const std::map<std::string, std::string>&);

public:
  ESRHeaderAP();
  ESRHeaderAP(const std::map<std::string, std::string>&);
  ESRHeaderAP(const ESRHeaderAP&) = default;

  virtual void Print(Option_t* options = "") const final;

  ClassDef(ESRHeaderAP, 1);
};


// ------------------------------------------------------------
// ------------------------------------------------------------
// ------------------------------------------------------------
class ESRHeaderDT : public ESRHeaderElement
{
  int length_; // this should be same as "data length"
  virtual void set_val(const std::map<std::string, std::string>&);

public:
  ESRHeaderDT();
  ESRHeaderDT(const std::map<std::string, std::string>&);
  ESRHeaderDT(const ESRHeaderDT&) = default;

  int GetLength() const;
  void SetLength(int val);

  virtual void Print(Option_t* options = "") const final;


  ClassDef(ESRHeaderDT, 1);
};

#endif
