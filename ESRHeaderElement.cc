#include "ESRHeaderElement.hh"
#include <cmath>
#include "TError.h"


ClassImp(ESRHeaderElement)
ClassImp(ESRHeaderDH)
ClassImp(ESRHeaderGP)
ClassImp(ESRHeaderSP)
ClassImp(ESRHeaderAP)
ClassImp(ESRHeaderDT)


// ------------------------------------------------------------
// ------------------------------------------------------------
// ------------------------------------------------------------
ESRHeaderElement::ESRHeaderElement()
{}

ESRHeaderElement::ESRHeaderElement(const std::map<std::string, std::string>& key_val)
{
  set_val(key_val);
}

std::string ESRHeaderElement::mysubstr(std::string val, size_t pos)
{
  if(pos > val.size())
    return std::string(val);
  else
    return val.substr(pos);
}

std::string ESRHeaderElement::mysubstr(std::string val, size_t pos1, size_t pos2)
{
  if(pos1 > pos2)
    return std::string(val);

  if(pos2 > val.size())
    return std::string(val);
  else
    return val.substr(pos1, pos2);
}


// helper functions
std::string ESRHeaderElement::mystos(const std::map<std::string, std::string>& key_val,
                                     std::string key)
{
  if(key_val.count(key))
    return key_val.at(key);
  else
    return std::string{""};
}

int ESRHeaderElement::mystoi(const std::map<std::string, std::string>& key_val,
                             std::string key)
{
  if(key_val.count(key))
    {
      try
        {
          return std::stoi(key_val.at(key));
        }
      catch(std::invalid_argument& e)
        {
          Info("ESRHeaderElement::mystoi", "fail to convert \"%s\" to double",
               key_val.at(key).c_str());
          return 0;
        }
    }
  else
    return 0;
}

int ESRHeaderElement::mystoi(const std::string& val)
{
  try
    {
      return std::stoi(val);
    }
  catch(std::invalid_argument& e)
    {
      Info("ESRHeaderElement::mystoi", "fail to convert \"%s\" to double", val.c_str());
      return 0;
    }
}

double ESRHeaderElement::mystod(const std::map<std::string, std::string>& key_val,
                                std::string key)
{
  if(key_val.count(key))
  {
    try
    {
      return std::stod(key_val.at(key));
    }
  catch(std::invalid_argument& e)
    {
      Info("ESRHeaderElement::mystoi", "fail to convert \"%s\" to double.",
           key_val.at(key).c_str());
      return double{0};
    }
  }
  return double{0};
}

double ESRHeaderElement::mystod(const std::string& val)
{
  try
    {
      return std::stod(val);
    }
  catch(std::invalid_argument& e)
    {
      Info("ESRHeaderElement::mystod", "fail to convert \"%s\" to double", val.c_str());
      return double{0};
    }
}


void ESRHeaderElement::set_val(const std::map<std::string, std::string>&)
{}; // virtual. override by derived class

void ESRHeaderElement::Print(Option_t*) const
{
  std::cout << "overrideしね❤" << std::endl;
};


// ------------------------------------------------------------
// ------------------------------------------------------------
// ------------------------------------------------------------
ESRHeaderDH::ESRHeaderDH() :
  ESRHeaderElement(),
  file_name_(""), data_number_(0), data_length_(0), data_sort_(""),
  x_range_min_(0), x_range_(0), x_unit_(""), minmax_value_(0, 0),
  y_unit_(""), x_view_(0, 0), y_view_(0, 0), type_("")
{}

ESRHeaderDH::ESRHeaderDH(const std::map<std::string, std::string>& key_val) :
  ESRHeaderElement(key_val),
  file_name_(""), data_number_(0), data_length_(0), data_sort_(""),
  x_range_min_(0), x_range_(0), x_unit_(""), minmax_value_(0, 0),
  y_unit_(""), x_view_(0, 0), y_view_(0, 0), type_("")
{
  set_val(key_val);
}

void ESRHeaderDH::set_val(const std::map<std::string, std::string>& key_val)
{
  file_name_ = this->mystos(key_val, "file name");
  data_number_ = this->mystoi(key_val, "data number");
  data_length_ = this->mystod(key_val, "data length");
  data_sort_ = this->mystos(key_val, "data length");
  x_range_min_ = this->mystod(key_val, "x-range min");
  x_range_ = this->mystod(key_val, "x-range");
  x_unit_ = this->mystos(key_val, "x unit");
  minmax_value_ = {this->mystod(key_val, "min value"), this->mystod(key_val, "max value")};
  y_unit_ = this->mystos(key_val, "y unit");
  x_view_ = {this->mystod(key_val, "x-view min"), this->mystod(key_val, "x-view max")};
  y_view_ = {this->mystod(key_val, "y-view min"), this->mystod(key_val, "y-view max")};
  type_ = this->mystos(key_val, "type");
}

void ESRHeaderDH::SetDataLength(int val) {data_length_ = val;};

void ESRHeaderDH::SetXrangeMin(double val) {x_range_min_ = val;};

void ESRHeaderDH::SetXrange(double val) {x_range_ = val;};

int ESRHeaderDH::GetDataLength() const {return data_length_;};


std::pair<double, double> ESRHeaderDH::GetXrange() const
{
  return std::pair<double, double>(x_range_min_, x_range_min_ + x_range_);
}

std::pair<double, double> ESRHeaderDH::GetYrange() const {return y_view_;}

std::string ESRHeaderDH::GetFileName() const {return file_name_;}

void ESRHeaderDH::Print(Option_t*) const
{
  std::cout << "data length: " << data_length_ << std::endl
            << "x range min/x range: " << x_range_min_ << " / " << x_range_ << std::endl
            << "x view: " << x_view_.first << ", " << x_view_.second << std::endl
            << "y view: " << y_view_.first << ", " << y_view_.second << std::endl;
}

// ------------------------------------------------------------
// ------------------------------------------------------------
// ------------------------------------------------------------
ESRHeaderGP::ESRHeaderGP()
  : ESRHeaderElement(), title_(), sample_name_(""), comment_(""), system_name_(""),
    version_(""), date_(""), origin_file_(""), prev_file_("")
{}

ESRHeaderGP::ESRHeaderGP(const std::map<std::string, std::string>& key_val)
  : ESRHeaderElement(key_val), title_(), sample_name_(""), comment_(""), system_name_(""),
    version_(""), date_(""), origin_file_(""), prev_file_("")
{
  set_val(key_val);
}

void ESRHeaderGP::set_val(const std::map<std::string, std::string>& key_val)
{
  title_ = this->mystos(key_val, "title");
  sample_name_ = this->mystos(key_val, "sample name");
  comment_ = this->mystos(key_val, "comment");
  system_name_ = this->mystos(key_val, "system name");
  version_ = this->mystos(key_val, "version");
  date_ = this->mystos(key_val, "date");
  origin_file_ = this->mystos(key_val, "origin file");
  prev_file_ = this->mystos(key_val, "prev. file");
}

std::string ESRHeaderGP::GetDate() const {return date_;};
std::string ESRHeaderGP::GetFileTitle() const {return title_;};
std::string ESRHeaderGP::GetSampleName() const {return sample_name_;};
std::string ESRHeaderGP::GetComment() const {return comment_;};

void ESRHeaderGP::SetDate(std::string val) {date_ = val;};

void ESRHeaderGP::Print(Option_t*) const
{
  std::cout << "title: " << title_ << std::endl
            << "date: " << date_ << std::endl;
}


// ------------------------------------------------------------
// ------------------------------------------------------------
// ------------------------------------------------------------
ESRHeaderSP::ESRHeaderSP()
  : ESRHeaderElement(), type_(""), center_field_(0), zero_(0), zero2_(0),
    rcv_mode_(""), reserved_area_(""), SW(), MW(), TMPR()
{}

ESRHeaderSP::ESRHeaderSP(const std::map<std::string, std::string>& key_val)
  : ESRHeaderElement(key_val), type_(""), center_field_(0), zero_(0), zero2_(0),
    rcv_mode_(""), reserved_area_(""), SW(), MW(), TMPR()
{
  set_val(key_val);
}

void ESRHeaderSP::set_val(const std::map<std::string, std::string>& key_val)
{
  rcv_mode_ = mystos(key_val, "receiver mode");
  rcv_mode2_ = mystos(key_val, "receiver mode2");
  reserved_area_ = mystos(key_val, "reserved area");
  center_field_ = mystod(mysubstr(mystos(key_val, "center field"), 2));

  // sweep parameters
  SW.control = mysubstr(mystos(key_val, "sweep control"), 2);
  SW.time = mystod(mysubstr(mystos(key_val, "sweep time"), 2));
  SW.width = {mystod(mysubstr(mystos(key_val, "sweep width(fine)"), 2)),
              mystod(mysubstr(mystos(key_val, "sweep width(coar)"), 2))};
  SW.mod_freq = mystod(mysubstr(mystos(key_val, "modulation freq."), 2));
  SW.phase = {mystod(mysubstr(mystos(key_val, "phase"), 2)),
              mystod(mysubstr(mystos(key_val, "phase (fine)"), 2))};
  SW.phase2 = {mystod(mysubstr(mystos(key_val, "phase2 (fine)"), 2)), 0};
  SW.mod_width = {mystod(mysubstr(mystos(key_val, "mod. width(fine)"),2)),
                  mystod(mysubstr(mystos(key_val, "mod. width(coarse)"),2))};
  SW.amp1 = {mystod(mysubstr(mystos(key_val, "amplitude(fine)"), 2)),
             mystod(mysubstr(mystos(key_val, "amplitude(coarse)"),2))};
  SW.amp2 = {mystod(mysubstr(mystos(key_val, "amplitude2(fine)"), 2)),
             mystod(mysubstr(mystos(key_val, "amplitude2(coars)"), 2))};
  SW.tc1 = mystod(mysubstr(mystos(key_val, "time constant"), 2));
  SW.tc2 = mystod(mysubstr(mystos(key_val, "time constant2"), 2));

  // zero
  zero_ = mystoi(mysubstr(mystos(key_val, "zero"), 2));
  zero2_ = mystoi(mysubstr(mystos(key_val, "zero2"), 2));

  // microwve
  MW.freq = mystod(mysubstr(mystos(key_val, "micro frequency"), 2));
  MW.freq_unit = mysubstr(mystos(key_val, "micro freq. unit"), 2);
  MW.power = mystod(mysubstr(mystos(key_val, "micro power"), 2));
  MW.pwr_unit = mysubstr(mystos(key_val, "micro power unit"), 2);
  MW.phase = mystoi(mysubstr(mystos(key_val, "micro phase"), 2));
  MW.coupling = mystoi(mysubstr(mystos(key_val, "micro coupling"), 2));
  MW.is_30db = (mysubstr(mystos(key_val, "micro 30db"), 2) == "on");
  MW.is_ref = (mysubstr(mystos(key_val, "micro ref"), 2) == "on");
  MW.is_gunp = (mysubstr(mystos(key_val, "micro gunp"), 2) == "on");

  // temperature
  TMPR.vt_type = (mystos(key_val, "vt type"));
  TMPR.temp_control = (mystos(key_val, "temp. control") == "on");
  TMPR.temperature = (mystos(key_val, "temperature").find("RT") != std::string::npos)?
    300 : mystod(mystos(key_val, "temperature"));
  TMPR.tmpr_unit = mystos(key_val, "temperature unit");
}

ESRHeaderSP::Sweep ESRHeaderSP::GetSW() const {return SW;};
ESRHeaderSP::MicroWave ESRHeaderSP::GetMW() const {return MW;};
ESRHeaderSP::Temperature ESRHeaderSP::GetTMPR() const {return TMPR;};


std::pair<double, double> ESRHeaderSP::GetAmplitude(int type) const
{
  if(type < 1 or type > 2)
    {
      Warning("ESRHeaderSP::GetAmplitude", "type should be 1 or 2. Force 'type = 1'");
      type = 1;
    }

  return (type == 1)? this->SW.amp1 : this->SW.amp2;
}

void ESRHeaderSP::SetAmplitude(std::pair<double, double> amp, int type)
{
  if(type < 1 or type > 2)
    {
      Warning("ESRHeaderSP::GetAmplitude", "type should be 1 or 2. Force 'type = 1'");
      type = 1;
    }

  if(type == 1)
    this->SW.amp1 = amp;
  else if(type == 2)
    this->SW.amp2 = amp;

  return;
}

double ESRHeaderSP::GetGain() const
{
  return SW.amp1.first * std::pow(10, SW.amp1.second);
}


void ESRHeaderSP::Print(Option_t*) const
{
  std::cout << "Sweep"  << std::endl
            << "* time: " << SW.time << std::endl
            << "* width: " << SW.width.first << ", " << SW.width.second << std::endl
            << "* amp: " << SW.amp1.first << " x 10^" << SW.amp1.second << std::endl
            << "* time constant" << SW.tc1 << std::endl
            << "Microwave" << std::endl
            << "* frequency: " << MW.freq << " " << MW.freq_unit << std::endl
            << "* power: " << MW.power << " " << MW.pwr_unit << std::endl
            << "Temperature" << std::endl
            << "* control is on? " << std::boolalpha << TMPR.temp_control << std::endl
            << "* temperature: " << TMPR.temperature << " " << TMPR.tmpr_unit << std::endl;
}


// ------------------------------------------------------------
// ------------------------------------------------------------
// ------------------------------------------------------------
ESRHeaderAP::ESRHeaderAP()
  : ESRHeaderElement(), accumu_count_(0), delay_time_(0), interval_time_(0),
    reserved_int_(0), sampling_time_(0), reserved_float_(0), accumu_mode_(""),
    baseline_(""), sampling_mode_(""), signal_trigger_(""),
    ref_file_name_(""), reserved_char_("")
{}

ESRHeaderAP::ESRHeaderAP(const std::map<std::string, std::string>& key_val)
  : ESRHeaderElement(key_val), accumu_count_(0), delay_time_(0), interval_time_(0),
    reserved_int_(0), sampling_time_(0), reserved_float_(0), accumu_mode_(""),
    baseline_(""), sampling_mode_(""), signal_trigger_(""),
    ref_file_name_(""), reserved_char_("")
{
  set_val(key_val);
}

void ESRHeaderAP::set_val(const std::map<std::string, std::string>& key_val)
{
  accumu_count_ = this->mystoi(key_val, "accumulation count");
  delay_time_ = this->mystoi(key_val, "delay time");
  interval_time_ = this->mystoi(key_val, "interval time");
  reserved_int_ = this->mystoi(key_val, "reserved(int)");
  sampling_time_ = this->mystoi(key_val, "sampling time");
  reserved_float_ = this->mystod(key_val, "reserved(float)"); // inplicit conversion from float to double
  accumu_mode_ = this->mystos(key_val, "accumulation mode");
  baseline_ = this->mystos(key_val, "baseline");
  sampling_mode_ = this->mystos(key_val, "sampling mode");
  signal_trigger_ = this->mystos(key_val, "signal trigger");
  ref_file_name_ = this->mystos(key_val, "reference file name");
  reserved_char_ = this->mystos(key_val, "reserved(char)");
}

void ESRHeaderAP::Print(Option_t*) const
{
  std::cout << "sampling time: " << sampling_time_ << std::endl;
}


// ------------------------------------------------------------
// ------------------------------------------------------------
// ------------------------------------------------------------
ESRHeaderDT::ESRHeaderDT()
  : ESRHeaderElement(), length_(0)
{}

ESRHeaderDT::ESRHeaderDT(const std::map<std::string, std::string>& key_val)
  : ESRHeaderElement(key_val), length_(0)
{
  set_val(key_val);
}

int ESRHeaderDT::GetLength() const {return length_;};

void ESRHeaderDT::SetLength(int val) {length_ = val;};

void ESRHeaderDT::set_val(const std::map<std::string, std::string>& key_val)
{
  length_ = this->mystoi(key_val, "length");
}

void ESRHeaderDT::Print(Option_t*) const
{
  std::cout << "length: " << length_ << std::endl;
}
