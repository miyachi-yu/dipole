#ifndef ESRHEADERELEMENT_HH
#define ESRHEADERELEMENT_HH

#include <iostream>
#include <fstream>
#include <memory>
#include <map>

#include "TObject.h"

/**
   Classes manage header infomation.
   See each class definition.
 */

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

  /**
     sweep structure.
   */
  struct Sweep
  {
    /** sweep control  */
    std::string control = ""; //swquit
    /** sweep time. unit is second.   ...maybe. not sure... */
    double time = 0;
    /** sweep width {fine, coarse} */
    std::pair<double, double> width = {0, 0};
    /** modulation frequency  */
    double mod_freq = 0;
    /** phase {fine, coarse} */
    std::pair<double, double> phase = {0, 0};
    /** phase2 {fine, coasrse} */
    std::pair<double, double> phase2 = {0, 0};
    /** modulation width {fine, coarse}  */
    std::pair<double, double> mod_width = {0, 0};
    /** amplitude {fine, coarse} */
    std::pair<double, double> amp1 = {0, 0};
    /** amplitude2 {fine, coarse} */
    std::pair<double, double> amp2 = {0, 0};
    /** time constant */
    double tc1 = 0;
    /** time constant2 */
    double tc2 = 0;
  } SW_;

  /**
     microwave structure.
   */
  struct MicroWave
  {
    /** micro frequency */
    double freq = 0;
    /** micro freq. unit */
    std::string freq_unit = "";
    /** micro power */
    double power = 0;
    /** micro power unit */
    std::string pwr_unit = "";
    /** micro phase */
    int phase = 0;
    /** micro coupling */
    int coupling = 0;
    /** micro 30db */
    bool is_30db = false;
    /** micro ref */
    bool is_ref= false;
    /** micro gunp */
    bool is_gunp = false;
  } MW_;

  /**
     Temperature structure
   */
  struct Temperature
  {
    /** vt type */
    std::string vt_type = ""; // --
    /** temp. control */
    bool temp_control = false;
    /** temperature */
    double temperature = 20; // Celcius
    /** temperature unit */
    std::string tmpr_unit = ""; //
  } TMPR_;

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
