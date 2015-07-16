#include "ESR.hh"
#include "ESRHeader.hh"
#include "ESRHeaderElement.hh"
// #include "ESRUtil.hxx"

#include <fstream>
#include <stdexcept>
#include <iomanip>
#include <algorithm>
#include <ctime>   // time_t, tm
#include <cstring> // memset

#include "TError.h"
#include "TGraph.h"
#include "TAxis.h"
#include "TArrow.h"
#include "TFile.h"
#include "TTree.h"

ClassImp(ESR)


// graph style
GraphStyle::GraphStyle() :
  line_style(1), line_color(1), line_width(1),
  marker_style(20), marker_color(1), marker_size(1)
{}

GraphStyle::GraphStyle(int lc, int lw) :
  line_style(1), line_color(lc), line_width(lw),
  marker_style(20), marker_color(1), marker_size(1)
{};

GraphStyle::GraphStyle(const std::tuple<int, int, int, int, int, int>& conf) :
  line_style(std::get<0>(conf)), line_color(std::get<1>(conf)), line_width(std::get<2>(conf)),
  marker_style(std::get<3>(conf)), marker_color(std::get<4>(conf)), marker_size(std::get<5>(conf))
{}


// static variables
int ESR::nlines_header_txt_file = 77; // private
std::string ESR::date_format = "%Y/%m/%d %H:%M"; //   ~ is removed.
std::string ESR::x_axis_title = "Magnetic field (mT)";
std::string ESR::y_axis_title = "Amplitude";
GraphStyle ESR::gs_sig = {64, 2};
GraphStyle ESR::gs_sig_integ = {2, 3};
GraphStyle ESR::gs_sig_imag = {412, 1};
GraphStyle ESR::gs_sig_imag_integ = {807, 2};


// constructors
/**
   Default constructor.

   Info message is printed out if gIgnoreErrorLevel < gInfo
 */
ESR::ESR() :
  file_type_(-1), esr_header_(std::make_shared<ESRHeader>()),
  file_path_(""),data_length_(-1), reduction_factor_(0),
  xrange_{-1, -1}, yrange_{-1, -1}, date_(""), gain_(0),
  graph_(nullptr), graph_norm_(nullptr), graph_integ_(nullptr), graph_norm_integ_(nullptr),
  graph_imag_(nullptr), graph_imag_norm_(nullptr),
  graph_imag_integ_(nullptr), graph_imag_norm_integ_(nullptr)
{
  Info("ESR()", "default cnstr");
}

// normal
/**
   Normal constructor.
   @param reduction_factor how many points are merged every reduction factor.
   Every reduction factor, data points are grouped, calculating mean value for x nad y points.

   File can be text file or ROOT file.
   There are two format for text file: BOTH type is accepted.
   Raw binary data is acceptable.
 */
ESR::ESR(std::string file_path, int reduction_factor) :
  file_type_(-1), file_path_(file_path), data_length_(0), reduction_factor_(reduction_factor),
  xrange_{0, 0}, yrange_{0, 0}, date_(""), gain_(0),
  graph_(nullptr), graph_norm_(nullptr), graph_integ_(nullptr), graph_norm_integ_(nullptr),
  graph_imag_(nullptr), graph_imag_norm_(nullptr),
  graph_imag_integ_(nullptr), graph_imag_norm_integ_(nullptr)
{
  Info("ESR(std::string, int)", "cnstr");

  std::ifstream ifs(file_path_.c_str(), std::ifstream::binary);
  if(not CheckInputFile(ifs))
    return;

  CheckFileFormatType(ifs);

  if(file_type_ == -1)     // other file
    {
      return; // exit constructor
    }
  else if(file_type_ == 2) // ROOT file
    {
      ifs.close();
      LoadFromROOTFile();
    }
  else if(file_type_ != 2) // normal, wave format, and binary file
    {
      ifs.seekg(ifs.beg);
      raw_header_ = std::move(ParseHeader(ifs));
      MakeHeader(raw_header_);
      SetParams();
      ParseData(ifs);
      ifs.close();
    }

  // reduction and integral
  CheckReductionFactor();
  ReduceData();
  IntegrateData();
  MakeAllGraphs();
}

/**
   Copy constructor

   This is the practical and basic impliment.
   See the link at the move constructor.
 */
ESR& ESR::operator=(const ESR& esr)
{
  Info("operator=(const ESR&)", "copy");
  if(this != &esr)
    {
      file_type_ = esr.file_type_;

      if(esr.esr_header_)
        {
          esr_header_ = std::shared_ptr<ESRHeader>(new ESRHeader{*(esr.esr_header_.get())});
          // `- little bit tricky.
        }
      else
        {
          /* in case there is no ESRHeader, create new one. */
          esr_header_ = std::shared_ptr<ESRHeader>(new ESRHeader{});
        }

      file_path_ = esr.file_path_;
      data_length_ = esr.data_length_;
      reduction_factor_ = esr.reduction_factor_;
      xrange_ = esr.xrange_;
      yrange_ = esr.yrange_;
      date_ = esr.date_;
      gain_ = esr.gain_;

      // vector...
      vxdata_ = esr.vxdata_;
      vydata_ = esr.vydata_;
      vydata_integ_ = esr.vydata_integ_;
      vydata_norm_ = esr.vydata_norm_;
      vydata_norm_integ_ = esr.vydata_norm_integ_;

      vydata_imag_ = esr.vydata_imag_;
      vydata_imag_integ_ = esr.vydata_imag_integ_;
      vydata_imag_norm_ = esr.vydata_imag_norm_;
      vydata_imag_norm_integ_ = esr.vydata_imag_norm_integ_;

      vxdata_orig_ = esr.vxdata_orig_;
      vydata_orig_ = esr.vydata_orig_;
      vydata_norm_orig_ = esr.vydata_norm_orig_;
      vydata_imag_orig_ = esr.vydata_imag_orig_;
      vydata_imag_norm_orig_ = esr.vydata_imag_norm_orig_;

      // helper function to clone graph
      auto clone_graph = [&](const std::shared_ptr<TGraph>& gr) -> std::shared_ptr<TGraph>
        {
          if(not gr)
            return nullptr;

          auto gr_new = dynamic_cast<TGraph*>(gr->Clone());
          if(not gr_new)
            {
              Error("ESR(const ESR&)", "Graph is not derived from TGraph!?!?");
              return nullptr;
            }
          return std::shared_ptr<TGraph>(std::move(gr_new));
        };

      // clone graphs
      graph_  = clone_graph(esr.graph_);
      graph_norm_ = clone_graph(esr.graph_norm_);
      graph_integ_ = clone_graph(esr.graph_integ_);
      graph_norm_integ_ = clone_graph(esr.graph_norm_integ_);
      graph_imag_ = clone_graph(esr.graph_imag_);
      graph_imag_norm_ = clone_graph(esr.graph_imag_norm_);
      graph_imag_integ_ = clone_graph(esr.graph_imag_integ_);
      graph_imag_norm_integ_ = clone_graph(esr.graph_imag_norm_integ_);
    }

  return *this;
}

/**
   copy constructor

   \code{.cpp}
   ESR esr1 {"data1.txt"};
   auto esr2 = esr1;
   \endcode
 */
ESR::ESR(const ESR& esr)
{
  Info("ESR(const ESR&)", "operator=(const ESR&) is called.");
  *this = esr;
}

//
// move
/**
   move operator (constructor)

   steal enerything from original object.
   This is the practical impliment. see <a target="_blank" href="http://qiita.com/termoshtt/items/3397c149bf2e4ce07e6c#move-assignment-operator-%E7%A7%BB%E5%8B%95%E4%BB%A3%E5%85%A5%E6%BC%94%E7%AE%97%E5%AD%90">Qiita</a> or <a target="_blank" href="https://msdn.microsoft.com/ja-jp/library/dd293665.aspx">MSDN</a>
 */
ESR& ESR::operator=(ESR&& esr) noexcept
{
  Info("& operator=(ESR&&)", "move");

  if(this != &esr)
    {
      file_type_ = std::move(esr.file_type_);

      // header
      esr_header_ = std::move(esr.esr_header_);

      // variables
      file_path_ = std::move(esr.file_path_);
      data_length_ = std::move(esr.data_length_);
      reduction_factor_ = std::move(esr.reduction_factor_);
      xrange_ = std::move(esr.xrange_);
      yrange_ = std::move(esr.yrange_);
      gain_ = std::move(esr.gain_);

      // initialize member variable at oritinal esr obj.
      esr.data_length_ = -1;
      esr.reduction_factor_ = 0;
      esr.xrange_ = {-1, -1};
      esr.yrange_ = {-1, -1};
      esr.gain_ = 0;

      // vector
      vxdata_ = std::move(esr.vxdata_);
      vydata_ = std::move(esr.vydata_);
      vydata_integ_ = std::move(esr.vydata_norm_);
      vydata_norm_integ_ = std::move(esr.vydata_norm_integ_);

      vydata_imag_ = std::move(esr.vydata_imag_);
      vydata_imag_integ_ = std::move(esr.vydata_imag_integ_);
      vydata_imag_norm_ = std::move(esr.vydata_imag_norm_);
      vydata_imag_norm_integ_ = std::move(esr.vydata_imag_norm_integ_);

      // original
      vxdata_orig_ = std::move(vxdata_orig_);
      vydata_orig_ = std::move(vydata_orig_);
      vydata_norm_orig_ = std::move(vydata_norm_orig_);
      vydata_imag_orig_ = std::move(vydata_imag_orig_);
      vydata_imag_norm_orig_ = std::move(vydata_imag_norm_orig_);

      // graphs
      graph_ = std::move(esr.graph_);
      graph_norm_ = std::move(esr.graph_norm_);
      graph_integ_ = std::move(esr.graph_integ_);
      graph_norm_integ_ = std::move(esr.graph_norm_integ_);

      graph_imag_ = std::move(esr.graph_imag_);
      graph_imag_norm_ = std::move(esr.graph_imag_norm_);
      graph_imag_integ_ = std::move(esr.graph_imag_integ_);
      graph_imag_norm_integ_ = std::move(esr.graph_imag_norm_integ_);
    }

  return *this;
}

/**
   move.

   \code{.cpp}
   ESR esr1{"data.txt"};
   auto esr2 = std::move(esr1);
   \endcode
 */
ESR::ESR(ESR&& esr) noexcept
{
  Info("ESR(ESR&&)", "operator=(ESR&&) is called.");
  *this = std::move(esr);
}

/**
   destructor

   TGraph objects will be deleted safely and automatically
   IF there is no reference.
 */
ESR::~ESR()
{
  Info("~ESR()", "delete cnstr");

  // decrement reference count.
  // If there are objects refering to resource, destructor of TGraph would not be called.
  graph_.reset();
  graph_norm_.reset();
  graph_integ_.reset();
  graph_norm_integ_.reset();
  graph_imag_.reset();
  graph_imag_norm_.reset();
  graph_imag_integ_.reset();
  graph_imag_norm_integ_.reset();
}

// helper function
/**
   trim space, tab, cariiage return and line feed.

   See <a href="http://www.toptip.ca/2010/03/trim-leading-or-trailing-white-spaces.html"
   target="_blank">sample</a>
 */
std::string ESR::trim(const std::string& s)
{
  auto p = s.find_first_not_of(" \t\r\n");
  std::string st;
  if(p != s.npos)
    st = s.substr(p);

  p = st.find_last_not_of(" \t\r\n");
  if(p != std::string::npos)
    st = st.erase(p+1);
  else if(p == std::string::npos and st.size() == 1)
    st = "";

  return st;
}

/**
   replace 'rep' to 'to' in 's'
 */
std::string ESR::replace(const std::string& s, const char rep, const char to)
{
  std::string ss{s}; // copy

  for(auto& c: ss)
    {
      if(c == rep)
        c = to;
    }
  return ss;
}

/**
   parse header

   Change process according to file type.
   This may be done by another class because this code is little bit lengthy...
 */
std::map<std::string, std::string> ESR::ParseHeader(std::ifstream& ifs)
{
  if(file_type_ < 0)
    {
      Fatal("", "FUCK. this is not readble format.");
      throw std::runtime_error("fuck fuck fuck");
    }

  std::map<std::string, std::string> header_key_val;
  std::string buf;
  if(file_type_ == 0)
    {
      /* normal file */
      for(auto i = 0; i < nlines_header_txt_file; ++i)
        {
          std::getline(ifs, buf);
          auto ind = buf.find("=");
          if(ind == 0)
            continue;

          auto key = trim(replace(buf.substr(0, ind)));
          auto val = trim(replace(buf.substr(ind + 1)));
          header_key_val[key] = val;
        }
    }
  else if(file_type_ == 1)
    {
      /* wave format */
      ifs >> buf;
      ifs >> buf;

      header_key_val["data length"] = buf.substr(buf.find("=") + 1);
      header_key_val["length"] = header_key_val["data length"];

      std::getline(ifs, buf); // end of 1st line

      std::getline(ifs, buf); // l.2
      header_key_val["title"] = this->trim(this->replace(buf.substr(buf.find("=") + 1)));

      std::getline(ifs, buf); // l.3
      header_key_val["sample name"] = this->trim(this->replace(buf.substr(buf.find("=") + 1)));

      std::getline(ifs, buf); // l.4
      header_key_val["comment"] = this->trim(this->replace(buf.substr(buf.find("=") + 1)));

      std::getline(ifs, buf); // l.5
      header_key_val["date"] = this->trim(this->replace(buf.substr(buf.find("=") + 1)));

      // l.6
      ifs >> buf;
      header_key_val["center field"] = this->trim(buf);
      ifs >> buf;
      header_key_val["sweep time"] = this->trim(buf);
      ifs >> buf;
      header_key_val["receiver mode"] = this->trim(buf);
      ifs >> buf;
      header_key_val["modulation freq."] = this->trim(buf);
      ifs >> buf; // md1.0 x 1
      header_key_val["mod. width(fine)"] = this->trim(buf.substr(0, buf.find("x")));
      header_key_val["mod. width(coarse)"] = "md" +
        std::to_string(std::log10(std::stod(this->trim(buf.substr(buf.find("x") + 1)))));
      // fuckin' lengthy...

      std::getline(ifs, buf); // eol l.6

      ifs >> buf; // l.7
      header_key_val["amplitude(fine)"] = this->trim(buf.substr(0, buf.find("x")));
      header_key_val["amplitude(coarse)"] =
        std::to_string(std::log10(std::stod(this->trim(buf.substr(buf.find("x") + 1)))));

      ifs >> buf; // tc
      header_key_val["time constant"] = this->trim(buf);
      ifs >> buf; // uf
      header_key_val["micro frequency"] = this->trim(buf);
      ifs >> buf; //uf unit
      header_key_val["micro freq. unit"] = "UF" + this->trim(buf);
      ifs >> buf; // uf power
      header_key_val["micro power"] = this->trim(buf);
      ifs >> buf; // uf power unit
      header_key_val["micro power unit"] = "UP" + this->trim(buf);
      std::getline(ifs, buf); // eol 7

      std::getline(ifs, buf);
      header_key_val["accumulation count"] = this->trim(buf.substr(buf.find("=") + 1));
    }
  else if(file_type_ == 3)
    {
      // binary format.
      const int bsize = 1024;
      char buf[bsize];

      // helper function
      // fill up with null char.
      auto clear = [](char buf[], int size)->void
        {
          std::memset(buf, '\x00', size);
        };

      // start reading:
      // (sorry for ugly codes...)
      clear(buf, bsize);
      ifs.seekg(0x00);
      ifs.read(buf, 0x10);
      std::string data_head_type {buf}; //cAcqu
      header_key_val["type"] = buf;

      clear(buf, bsize);
      ifs.read(buf, 0x40);
      std::string file_name {buf};
      header_key_val["file name"] = file_name;

      clear(buf, bsize);
      ifs.seekg(0x56);
      ifs.read(buf, 4);
      auto data_size = *reinterpret_cast<int*>(buf);
      header_key_val["data length"] = std::to_string(data_size);
      header_key_val["length"] = std::to_string(data_size);

      ifs.seekg(0x60);
      ifs.read(buf, 0x10);
      std::string data_sort {buf};
      header_key_val["data sort"] = data_sort;

      ifs.seekg(0x70);
      ifs.read(buf, 0x04);
      auto x_range_min = *reinterpret_cast<float*>(buf);
      ifs.read(buf, 0x04);
      auto x_range = *reinterpret_cast<float*>(buf);

      header_key_val["x-range min"] = std::to_string(x_range_min);
      header_key_val["x-range"] = std::to_string(x_range);

      clear(buf, bsize);
      ifs.read(buf, 2);
      std::string x_unit {buf};
      header_key_val["x unit"] = x_unit;

      ifs.seekg(0xb4);
      ifs.read(buf, 4);
      auto x_view_min = *reinterpret_cast<float*>(buf);
      ifs.read(buf, 4);
      auto x_view_max = *reinterpret_cast<float*>(buf);
      ifs.read(buf, 4);
      auto y_view_min = *reinterpret_cast<float*>(buf);
      ifs.read(buf, 4);
      auto y_view_max = *reinterpret_cast<float*>(buf);
      header_key_val["x-view min"] = std::to_string(x_view_min);
      header_key_val["x-view max"] = std::to_string(x_view_max);
      header_key_val["y-view min"] = std::to_string(y_view_min);
      header_key_val["y-view max"] = std::to_string(y_view_max);

      /////  next
      ifs.seekg(0x18ac);
      ifs.read(buf, 4);
      std::string type {buf}; // tyFA
      header_key_val["type"] = "tyFA"; // oh.... duplicate...

      ifs.seekg(0x18fc);
      ifs.read(buf, 0x10);
      std::string center_field {buf};
      header_key_val["center field"] = center_field;

      clear(buf, bsize);
      ifs.seekg(0x190c);
      ifs.read(buf, 0x10);
      std::string sweep1 {buf};
      header_key_val["sweep width(fine)"] = sweep1;

      clear(buf, bsize);
      ifs.seekg(0x191c);
      ifs.read(buf, 0x10);
      std::string sweep2 {buf};
      header_key_val["sweep width(coar)"] = sweep2;

      clear(buf, bsize);
      ifs.seekg(0x194c);
      ifs.read(buf, 4);
      std::string sweep_time {buf};
      header_key_val["sweep time"] = sweep_time;

      clear(buf, bsize);
      ifs.seekg(0x197c);
      ifs.read(buf, 0x10);
      std::string sweep_control {buf};
      header_key_val["sweep control"] = sweep_control;

      ////// frequency
      clear(buf, bsize);
      ifs.seekg(0x1a5c);
      ifs.read(buf, 0x10);
      std::string mod_frq {buf};
      header_key_val["modulation freq."] = mod_frq;

      clear(buf, bsize);
      ifs.read(buf, 0x10);
      std::string mod_width_fine {buf};
      header_key_val["mod. width(fine)"] = mod_width_fine;

      clear(buf, bsize);
      ifs.read(buf, 0x10);
      std::string mod_width_coarse {buf};
      header_key_val["mod. width(coarse)"] = mod_width_coarse;

      clear(buf, bsize);
      ifs.read(buf, 0x10);
      std::string phase {buf};
      header_key_val["phase"] = phase;

      clear(buf, bsize);
      ifs.read(buf, 0x10);
      std::string receiver_mode {buf};
      header_key_val["receiver mode"] = receiver_mode;

      clear(buf, bsize);
      ifs.read(buf, 0x10);
      std::string phase_fine {buf};
      header_key_val["phase (fine)"] = phase_fine;

      clear(buf, bsize);
      ifs.seekg(0x1abc);
      ifs.read(buf, 0x10);
      std::string amplitude_fine {buf};
      header_key_val["amplitude(fine)"] = amplitude_fine;

      clear(buf, bsize);
      ifs.read(buf, 0x10);
      std::string amplitude_coarse {buf};
      header_key_val["amplitude(coarse)"] = amplitude_coarse;

      clear(buf, bsize);
      ifs.read(buf, 0x10);
      std::string time_constant {buf};
      header_key_val["time constant"] = time_constant;

      clear(buf, bsize);
      ifs.read(buf, 16);
      std::string zero {buf};
      header_key_val["zero"] = zero;

      clear(buf, bsize);
      ifs.seekg(0x1afb);
      ifs.read(buf, 0x10);
      std::string receiver_mode1 {buf}; // rmsecond
      header_key_val["receiver mode2"] = receiver_mode1;

      clear(buf, bsize);
      ifs.seekg(0x1b0c);
      ifs.read(buf, 0x10);
      std::string phase2_fine {buf};
      header_key_val["phase2 (fine)"] = phase2_fine;

      clear(buf, bsize);
      ifs.seekg(0x1b1c);
      ifs.read(buf, 0x10);
      std::string amplitude2_fine {buf};
      header_key_val["amplitude2(fine)"] = amplitude2_fine;

      clear(buf, bsize);
      ifs.read(buf, 0x10);
      std::string amplitude2_coarse {buf};
      header_key_val["amplitude2(coars)"] = amplitude2_coarse;

      clear(buf, bsize);
      ifs.read(buf, 0x10);
      std::string time_constant2 {buf};
      header_key_val["time constant2"] = time_constant2;

      // microwave
      clear(buf, bsize);
      ifs.seekg(0x1bec);
      ifs.read(buf, 0x08);
      std::string shf {buf};

      clear(buf, bsize);
      ifs.read(buf, 0x10);
      std::string mw_frq {buf};
      header_key_val["micro frequency"] = mw_frq;

      clear(buf, bsize);
      ifs.read(buf, 0x08);
      std::string mw_frq_unit {buf};
      header_key_val["micro freq. unit"] = mw_frq_unit;

      clear(buf, bsize);
      ifs.read(buf, 0x10);
      std::string mw_power {buf};
      header_key_val["micro power"] = mw_power;

      clear(buf, bsize);
      ifs.read(buf, 0x08);
      std::string mw_power_unit {buf};
      header_key_val["micro power unit"] = mw_power_unit;

      clear(buf, bsize);
      ifs.read(buf, 0x10);
      std::string mw_phase {buf};
      header_key_val["micro phase"] = mw_phase;

      clear(buf, bsize);
      ifs.seekg(0x1cb4);
      ifs.read(buf, 0x08);
      std::string gu {buf};
      header_key_val["micro gunp"] = gu;

      clear(buf, bsize);
      ifs.read(buf, 0x08);
      std::string re {buf};
      header_key_val["micro ref"] = re;

      clear(buf, bsize);
      ifs.read(buf, 0x08);
      std::string db30 {buf};
      header_key_val["micro 30db"] = db30;

      /// acq param
      clear(buf, bsize);
      ifs.seekg(0x1eac);
      ifs.read(buf, 0x10);
      std::string vt {buf};
      header_key_val["vt type"] = vt;

      clear(buf, bsize);
      ifs.read(buf, 0x10);
      std::string tt {&buf[0] + 2}; // skip the first two char
      header_key_val["temperature"] = tt;

      clear(buf, bsize);
      ifs.seekg(0x1efc);
      ifs.read(buf, 0x04);
      std::string tu {&buf[0] + 2}; // tempr. unit
      header_key_val["temperature unit"] = tu;

      clear(buf, bsize);
      ifs.seekg(0x205c);
      ifs.read(buf, 0x10);
      std::string date {buf};
      header_key_val["date"] = date;

      clear(buf, bsize);
      ifs.seekg(0x20b8);
      ifs.read(buf, 0x08);
      std::string acc_mode {buf};
      header_key_val["accumulation mode"] = acc_mode;

      clear(buf, bsize);
      ifs.seekg(0x20c0);
      ifs.read(buf, 0x08);
      std::string base_line {buf};
      header_key_val["baseline"] = base_line;

      clear(buf, bsize);
      ifs.read(buf, 0x08);
      std::string sampling_mode {buf};
      header_key_val["sampling mode"] = sampling_mode;

    }
  else
    {
      // never happen.
    }

  return std::move(header_key_val);
}

/**
   make header instance by parsed key data.
 */
void ESR::MakeHeader(const std::map<std::string, std::string>& key_val)
{
  esr_header_ = std::make_shared<ESRHeader>(key_val);
}

/**
   parse data

   read as double. Original data in a file is float.
 */
void ESR::ParseData(std::ifstream& ifs)
{
  if(file_type_ == 0)
    {
      // normal
      std::string buf;
      std::getline(ifs, buf);
      std::getline(ifs, buf);

      // reserve buffer of vectors
      vydata_norm_.reserve(data_length_);
      vxdata_orig_.reserve(data_length_);
      vydata_orig_.reserve(data_length_);
      vydata_norm_orig_.reserve(data_length_);

      // real part
      double val = 0;
      double dx = (xrange_.second - xrange_.first) / static_cast<double>(data_length_);
      for(auto index = 0; index < data_length_; ++index)
        {
          std::getline(ifs, buf);
          val = std::stod(buf); // slightly danger..
          try
            {
              val = std::stod(buf);
            }
          catch(std::invalid_argument& e)
            {
              std::cerr << "On l. " << __LINE__ << " in " << __FILE__ << ": "
                        << "fail to convert \"" << buf << "\"" << std::endl;
              throw e;
            }
          catch(std::out_of_range& e)
            {
              std::cerr << "On l. " << __LINE__ << " in " << __FILE__ << ": "
                        << "fail to convert \"" << buf << "\"" << std::endl;
              throw e;
            }

          vydata_orig_.push_back(val);
          vydata_norm_orig_.push_back(val / gain_);
          vxdata_orig_.push_back(xrange_.first + index * dx);
        }

      // reserve
      vydata_imag_.reserve(data_length_);
      vydata_imag_norm_.reserve(data_length_);
      vydata_imag_orig_.reserve(data_length_);
      vydata_imag_norm_orig_.reserve(data_length_);

      // imaginary part
      std::getline(ifs, buf);
      if(buf.find("Imaginary") == buf.npos)
        return;

      for(auto index = 0; index < data_length_; ++index)
        {
          std::getline(ifs, buf);
          try
            {
              val = std::stod(buf);
            }
          catch(std::invalid_argument& e)
            {
              std::cerr << "On l. " << __LINE__ << " in " << __FILE__ << ": "
                        << "fail to convert \"" << buf << "\"" << std::endl;
              throw e;
            }
          catch(std::out_of_range& e)
            {
              std::cerr << "On l. " << __LINE__ << " in " << __FILE__ << ": "
                        << "fail to convert \"" << buf << "\"" << std::endl;
              throw e;
            }

          vydata_imag_orig_.push_back(val);
          vydata_imag_norm_orig_.push_back(val / gain_);
        }
    }
  else if(file_type_ == 1)
    {
      // wave format
      std::string buf;
      std::getline(ifs, buf);
      std::getline(ifs, buf);

      // reserve buffer of vectors
      vxdata_orig_.reserve(data_length_);
      vydata_orig_.reserve(data_length_);
      vydata_norm_orig_.reserve(data_length_);

      auto val = 0.0;
      for(auto il = 0; il < data_length_; ++il)
        {
          // read x
          ifs >> buf;
          val = std::stod(buf);
          try
            {
              val = std::stod(buf);
            }
          catch(std::invalid_argument& e)
            {
              std::cerr << "On l. " << __LINE__ << " in " << __FILE__ << ": "
                        << "fail to convert \"" << buf << "\"" << std::endl;
              throw e;
            }
          catch(std::out_of_range& e)
            {
              std::cerr << "On l. " << __LINE__ << " in " << __FILE__ << ": "
                        << "fail to convert \"" << buf << "\"" << std::endl;
              throw e;
            }
          vxdata_orig_.push_back(val);

          // read y
          ifs >> buf;
          val = std::stod(buf);
          try
            {
              val = std::stod(buf);
            }
          catch(std::invalid_argument& e)
            {
              std::cerr << "On l. " << __LINE__ << " in " << __FILE__ << ": "
                        << "fail to convert \"" << buf << "\"" << std::endl;
              throw e;
            }
          catch(std::out_of_range& e)
            {
              std::cerr << "On l. " << __LINE__ << " in " << __FILE__ << ": "
                        << "fail to convert \"" << buf << "\"" << std::endl;
              throw e;
            }
          vydata_orig_.push_back(val);
          vydata_norm_orig_.push_back(val / gain_);
        }

      // imaginary part
      vydata_imag_.reserve(data_length_);
      vydata_imag_norm_.reserve(data_length_);
      vydata_imag_orig_.reserve(data_length_);
      vydata_imag_norm_orig_.reserve(data_length_);

      std::getline(ifs, buf); // remove \n
      std::getline(ifs, buf); // ===== CH2 data Wave No.1 =====
      std::getline(ifs, buf); // mT            Intensity
      for(auto il = 0; il < data_length_; ++il) /// il: index line
        {
          ifs >> buf; // dump. same as x in real part.
          ifs >> buf;
          try
            {
              val = std::stod(buf);
            }
          catch(std::invalid_argument& e)
            {
              std::cerr << "On l. " << __LINE__ << " in " << __FILE__ << ": "
                        << "fail to convert \"" << buf << "\"" << std::endl;
              throw e;
            }
          catch(std::out_of_range& e)
            {
              std::cerr << "On l. " << __LINE__ << " in " << __FILE__ << ": "
                        << "fail to convert \"" << buf << "\"" << std::endl;
              throw e;
            }

          vydata_imag_orig_.push_back(val);
          vydata_imag_norm_orig_.push_back(val / gain_);
        }

      // set parameters
      if(data_length_ > 0)
        {
          esr_header_->GetDataHead()->SetXrangeMin(vxdata_orig_.front());
          esr_header_->GetDataHead()->SetXrange(vxdata_orig_.back() - vxdata_orig_.front());

          // here call once
          SetParams();
        }
    }
  else if(file_type_ == 2)
    {
      ; // do nothing
    }
  else if(file_type_ == 3)
    {
      // binary file

      /* this maybe calculated by the following:
       positon = file_size - 4 * data_length * n_of_channel
       n_of_channel is normally 2: real and imaginary part.

       But may be this point is fixed, IMHO.
      */
      ifs.seekg(0x251c);

      // reserve
      vydata_norm_.reserve(data_length_);
      vxdata_orig_.reserve(data_length_);
      vydata_orig_.reserve(data_length_);
      vydata_norm_orig_.reserve(data_length_);

      // buffer
      char buf[4]; // small..? danger?

      /*
        fortunately endian is small: no byte swap needed.
       */

      // real part
      double val = 0;
      double dx = (xrange_.second - xrange_.first) / static_cast<double>(data_length_);
      for(auto index = 0; index < data_length_; ++index)
        {
          ifs.read(buf, 4);
          val = static_cast<double>(*reinterpret_cast<float*>(buf));

          vydata_orig_.push_back(val);
          vydata_norm_orig_.push_back(val / gain_);
          vxdata_orig_.push_back(xrange_.first + index * dx);
        }

      // reserve
      vydata_imag_.reserve(data_length_);
      vydata_imag_norm_.reserve(data_length_);
      vydata_imag_orig_.reserve(data_length_);
      vydata_imag_norm_orig_.reserve(data_length_);

      for(auto index = 0; index < data_length_; ++index)
        {
          ifs.read(buf, 4);
          val = static_cast<double>(*reinterpret_cast<float*>(buf));  // ugly

          vydata_imag_orig_.push_back(val);
          vydata_imag_norm_orig_.push_back(val / gain_);
        }
    }
  else
    {
      // never happen
    }
}

/**
   Load data from ROOT file.
 */
void ESR::LoadFromROOTFile()
{
  auto tf = std::unique_ptr<TFile>(TFile::Open(file_path_.c_str()));
  auto tree_header = dynamic_cast<TTree*>(tf->Get("header"));
  auto tree_data = dynamic_cast<TTree*>(tf->Get("data"));

  if(not (tree_header and tree_data))
    {
      Fatal("LoadFromROOTFile", "There is no tree of header and data.");
      tf->Close();
      return;
    }

  // read header
  std::pair<std::string, std::string>* pinfo;
  tree_header->SetBranchAddress("info", &pinfo);
  for(auto ient = 0ll, nent = tree_header->GetEntries(); ient < nent; ++ient)
    {
      tree_header->GetEntry(ient);
      raw_header_[(*pinfo).first] = (*pinfo).second;
    }
  MakeHeader(raw_header_);
  SetParams();

  // read data
  double x, y_real, y_imag, y_real_norm, y_imag_norm;
  tree_data->SetBranchAddress("x", &x);
  tree_data->SetBranchAddress("y", &y_real);
  tree_data->SetBranchAddress("y_imag", &y_imag);
  tree_data->SetBranchAddress("y_norm", &y_real_norm);
  tree_data->SetBranchAddress("y_imag_norm", &y_imag_norm);

  // data_length_ already set. here resize method is preferable.
  vxdata_orig_.resize(data_length_);
  vydata_orig_.resize(data_length_);
  vydata_norm_orig_.resize(data_length_);
  vydata_imag_orig_.resize(data_length_);
  vydata_imag_norm_orig_.resize(data_length_);

  for(auto ient = 0ll, nent = tree_data->GetEntries(); ient < nent; ++ient)
    {
      tree_data->GetEntry(ient);
      vxdata_orig_.at(ient) = x;
      vydata_orig_.at(ient) = y_real;
      vydata_norm_orig_.at(ient) = y_real_norm;
      vydata_imag_orig_.at(ient) = y_imag;
      vydata_imag_norm_orig_.at(ient) = y_imag_norm;
    }

  // close
  tf->Close();
}


/**
   Reading from internal header file, setting parameters.
 */
void ESR::SetParams()
{
  data_length_ = esr_header_->GetDataLength();
  date_ = esr_header_->GetDate();

  xrange_ = esr_header_->GetXrange();
  yrange_ = esr_header_->GetYrange();

  auto amp1s = esr_header_->GetAmplitude(1);
  gain_ = amp1s.first * std::pow(10, amp1s.second);
}


/**
   reduce data points.

   Original data is kept and unchanged by this operation.
   Calculating a mean value of all data points of which the number is reduction factor,
   make new vectors.
   （reduction factorで指定したデータ点の平均値をとり，vectorを作成し直す．）
 */
void ESR::ReduceData()
{
  // reduction
  std::vector<double> vxdata_red_, vydata_red_, vydata_norm_red_,
                      vydata_imag_red_, vydata_imag_norm_red_;

  // reserve size
  vxdata_red_.reserve(data_length_ + 1);
  vydata_red_.reserve(data_length_ + 1);
  vydata_norm_red_.reserve(data_length_ + 1);
  vydata_imag_red_.reserve(data_length_ + 1);
  vydata_imag_norm_red_.reserve(data_length_ + 1);

  for(auto i = 0, n = data_length_; i < n; i += reduction_factor_)
    {
      /* data reduction
         o take mean every 'reduction_factor' time.
      */
      auto nsize = 0.0;
      auto xsum = 0.0, ysum = 0.0, ynormsum = 0.0, yimagsum = 0.0, yimagnormsum = 0.0;
      for(auto j = 0; j < reduction_factor_; ++j)
        {
          auto index = i + j;
          if(index >= data_length_)
            continue;

          xsum += vxdata_orig_.at(index);
          ysum += vydata_orig_.at(index);
          ynormsum += vydata_norm_orig_.at(index);
          yimagsum += vydata_imag_orig_.at(index);
          yimagnormsum += vydata_imag_norm_orig_.at(index);

          ++nsize;
        }

      vxdata_red_.push_back(xsum / nsize);
      vydata_red_.push_back(ysum / nsize);
      vydata_norm_red_.push_back(ynormsum / nsize);
      vydata_imag_red_.push_back(yimagsum / nsize);
      vydata_imag_norm_red_.push_back(yimagnormsum / nsize);
    }

  // shrink
  vxdata_red_.shrink_to_fit();
  vydata_red_.shrink_to_fit();
  vydata_norm_red_.shrink_to_fit();
  vydata_imag_red_.shrink_to_fit();
  vydata_imag_norm_red_.shrink_to_fit();

  // move
  vxdata_ = std::move(vxdata_red_);
  vydata_ = std::move(vydata_red_);
  vydata_norm_ = std::move(vydata_norm_red_);
  vydata_imag_ = std::move(vydata_imag_red_);
  vydata_imag_norm_ = std::move(vydata_imag_norm_red_);
}

/**
   integrating data, which is reduced.

   Internally, this calls DoIntegral function.
 */
void ESR::IntegrateData()
{
  vydata_integ_ = std::move(DoIntegral(vxdata_, vydata_,
                                       xrange_.first, xrange_.second).second);
  vydata_norm_integ_ = std::move(DoIntegral(vxdata_, vydata_norm_,
                                            xrange_.first, xrange_.second).second);
  vydata_imag_integ_ = std::move(DoIntegral(vxdata_, vydata_imag_,
                                            xrange_.first, xrange_.second).second);
  vydata_imag_norm_integ_ = std::move(DoIntegral(vxdata_, vydata_imag_norm_,
                                                 xrange_.first, xrange_.second).second);
}

/**
   make all graphs by calling MakeGraph function.

   * raw signal
   * raw signal normalised by "normalise factor"
   * integrated raw signal
   * integrated raw signal normalised

   Graphs for imaginary part are also created.
 */
void ESR::MakeAllGraphs()
{
  // graphs
  graph_.reset(MakeGraph(vxdata_, vydata_));
  graph_norm_.reset(MakeGraph(vxdata_, vydata_norm_));
  graph_integ_.reset(MakeGraph(vxdata_, vydata_integ_));
  graph_norm_integ_.reset(MakeGraph(vxdata_, vydata_norm_integ_));
  //  imaginary
  graph_imag_.reset(MakeGraph(vxdata_, vydata_imag_));
  graph_imag_norm_.reset(MakeGraph(vxdata_, vydata_imag_norm_));
  graph_imag_integ_.reset(MakeGraph(vxdata_, vydata_imag_integ_));
  graph_imag_norm_integ_.reset(MakeGraph(vxdata_, vydata_imag_norm_integ_));

  // makeup real
  MakeupGraph(graph_, gs_sig, "");
  MakeupGraph(graph_norm_, gs_sig, "");
  MakeupGraph(graph_integ_, gs_sig_integ, "");
  MakeupGraph(graph_norm_integ_, gs_sig_integ, "");
  // makeup imaginary
  MakeupGraph(graph_imag_, gs_sig_imag, "");
  MakeupGraph(graph_imag_norm_, gs_sig_imag, "");
  MakeupGraph(graph_imag_integ_, gs_sig_imag_integ, "");
  MakeupGraph(graph_imag_norm_integ_, gs_sig_imag_integ, "");
}

/**
   check reduction factor and correct if necessary.
 */
void ESR::CheckReductionFactor()
{
  if(data_length_ <= 0)
    {
      Warning("CheckReductionFactor", "data length is invalid.");
      reduction_factor_ = 1;
    }

  if(reduction_factor_ <= 0 )
    {
      Warning("CheckReductionFactor", "reduction_factor must be greater than 0");
      reduction_factor_ = 1;
      Warning("ESR::CheckReductionFactor", "-> force to use %d", reduction_factor_);
    }

  if(reduction_factor_ > data_length_)
    {
      Warning("CheckReductionFactor",
              "reduction_factor must be less than data_length (%d)", data_length_);
      reduction_factor_ = data_length_;
      Warning("CheckReductionFactor", "-> force to use %d", reduction_factor_);
    }
}

/**
   check whether file exists or not
 */
bool ESR::CheckInputFile(std::ifstream& ifs)
{
  if(ifs.fail())
    {
      Error("CheckInputFile", "File open error");
      return false;
    }
  return true;
}

/**
   check file format. File type info is stored in the variable file_type_.

   -1: other file
    0: converted text file (80 lines header and data part)
    1: 'wave format' file (approx. 5 lines header and two columns data part)
    2: ROOT file created by ESR::Write() function.
    3: raw binary file

    Though raw binary file is accepted, some header infomation is missing.
    (Need more binary analysis... but it is not mandatory.)
 */
void ESR::CheckFileFormatType(std::ifstream& ifs)
{
  ifs.seekg(ifs.beg);
  std::string buf;
  std::getline(ifs, buf);

  if(buf.find("Data Head") != buf.npos)
    {
      file_type_ = 0;
      return;
    }
  else if(buf.find("wave") != buf.npos)
    {
      Info("ESR::CheckFileFormatType", "wave file format");
      file_type_ = 1;
      return;
    }

  // check whether ROOT file.
  ifs.seekg(ifs.beg);
  char cbuf[128];
  ifs.read(cbuf, 4); // root
  auto header_id = std::string{cbuf, 4};
  ifs.read(cbuf, 4); // version info stored as int with bigendian.

  auto char2int_be = [](const char* s) -> int
    {
      char buf[4];
      memcpy(buf, s, 4);
      std::swap(buf[0], buf[3]);
      std::swap(buf[1], buf[2]);
      return *(reinterpret_cast<int*>(buf));
    };
  int root_version = char2int_be(cbuf);

  // ROOT file created by older ROOT version is rejected.
  // (Maybe never happen, because this ESR class does not work older than ROOTv5
  //  and is intended to be compiled and used under ROOTv6)
  if(header_id == "root" and (root_version > 50000))
    {
      Info("ESR::CheckFileFormatType", "root file");
      file_type_ = 2;
      return;
    }

  // check binary file
  ifs.seekg(ifs.beg);
  ifs.read(cbuf, 0x10);
  header_id = std::string{cbuf};
  if(header_id.size() != 0)
    {
      Info("CheckFileFormatType", "binary file");
      file_type_ = 3;
      return;
    }

  Warning("CheckFileFormatType", "Strange file format");
  file_type_ = -1;
  return;
}

/**
   integrate data in specified range.

   User cannot call this; private function.
   This is called by functions related to signal integral.

   @param integral_constant treat the second component as integral constant if the first component is true
 */
std::pair<std::vector<double>, std::vector<double> > // <- fuckin writing. '>>' is acceptable at c++11 !!!
ESR::DoIntegral(const std::vector<double>& xs, const std::vector<double>& ys,
                double start, double end, const std::pair<bool, double> integral_constant) const
{
  if(xs.size() != ys.size())
    {
      Warning("DoIntegral", "data size inconsistent: x_size=%ld <-> y_size=%ld",
              xs.size(), ys.size());
      return std::make_pair(std::vector<double>{}, std::vector<double>{});
    }

  if((start < xrange_.first) or (end > xrange_.second))
    {
      Warning("DoIntegral",
              "Invalid integral range: it must be within %.3f to %.3f",
              xrange_.first, xrange_.second);
      return std::make_pair(std::vector<double>{}, std::vector<double>{});
    }

  if(start > end)
    {
      Warning("DoIntegral", "star > end ==> swap them");
      std::swap(start, end);
    }

  std::vector<double> xs_new, ys_integ;

  // reserve
  xs_new.reserve(xs.size());
  ys_integ.reserve(ys.size());

  double x, y_sum = 0;
  for(auto ip = 1ul, np = ys.size(); ip < np; ++ip)
    {
      x = xs.at(ip);
      if(x < start or x > end)
        continue;

      if(xs_new.size() == 0)
        {
          if(integral_constant.first)
            y_sum = integral_constant.second;
          else
            y_sum = ys.at(ip - 1);

          xs_new.push_back(xs.at(ip - 1));
          ys_integ.push_back(y_sum);
        }

      // trapezoidal integral
      y_sum += 0.5 * (ys.at(ip) + ys.at(ip - 1)) * (xs.at(ip) - xs.at(ip - 1));

      // push it
      ys_integ.push_back(y_sum);
      xs_new.push_back(x);
    }
  // shrink
  xs_new.shrink_to_fit();
  ys_integ.shrink_to_fit();

  return std::make_pair(std::move(xs_new), std::move(ys_integ));
}


/**
   create a graph integrated partially.

   @return shared_ptr of TGraph
 */
std::shared_ptr<TGraph> ESR::GetGraphIntegPart(double start, double end,
                                               const std::pair<bool, double> integral_constant,
                                               bool is_norm, bool is_imag) const
{
  const std::vector<double>* pvy;
  if(is_imag)
    pvy = is_norm? &vydata_imag_norm_ : &vydata_imag_;
  else
    pvy = is_norm? &vydata_norm_ : &vydata_;

  auto vxy = std::move(this->DoIntegral(vxdata_, *pvy, start, end, integral_constant));
  return std::shared_ptr<TGraph>(MakeGraph(vxy.first, vxy.second));
}


/**
   make graph from vector.

   @return raw TGraph pointer. should be treated by smart pointers.
 */
TGraph* ESR::MakeGraph(const std::vector<double>& x,
                       const std::vector<double>& y) const
{
  return new TGraph(x.size(), x.data(), y.data());
}

/**
   make up graph.

   Only basic cosmetic is done: title, name, axis, and line and marker configs.
 */
void ESR::MakeupGraph(const std::shared_ptr<TGraph>& gr,
                      const GraphStyle& gs, const std::string title) const
{
  gr->GetXaxis()->SetTitle(this->x_axis_title.c_str());
  gr->GetYaxis()->SetTitle(this->y_axis_title.c_str());

  gr->GetYaxis()->CenterTitle(true);

  gr->SetLineColor(gs.line_color);
  gr->SetLineWidth(gs.line_width);

  gr->SetMarkerStyle(20);

  if(title != "")
    gr->SetTitle(title.c_str());
}


/**
   Set new reduction factor.
   Data and graphs are updated.
 */
void ESR::SetReductionFactor(int reduction_factor)
{
  reduction_factor_ = reduction_factor;
  CheckReductionFactor();

  // recalculate data
  ReduceData();
  IntegrateData();
  MakeAllGraphs();
}

/**
   calculate integrated value.


 */
double ESR::Integrate(double start, double end, bool is_norm, bool is_imag,
                      const std::pair<bool, double> integral_constant) const
{
  const std::vector<double>* pvy; // Pointer of Vector Y
  if(is_imag)
    pvy = is_norm? &vydata_imag_norm_ : &vydata_imag_;
  else
    pvy = is_norm? &vydata_norm_ : &vydata_;

  const auto vxy = std::move(this->DoIntegral(vxdata_, *pvy, start, end, integral_constant));
  return std::accumulate(vxy.second.cbegin(), vxy.second.cend(), double{0});
}


/*
  getters
 */
/**
   return file type.
   - 0: text file
   - 1: wave format file
   - 2: ROOT file
   - 3: raw binary file
   - -1: other type
 */
int ESR::GetFileType() const {return file_type_;};

/**
   shared pointer of ESRHeader is returned.
 */
std::shared_ptr<ESRHeader> ESR::GetHeader() const {return esr_header_;}

std::pair<double, double> ESR::GetXrange() const {return esr_header_->GetXrange();}

std::pair<double, double> ESR::GetYrange() const {return esr_header_->GetYrange();}

double ESR::GetXmin() const {return xrange_.first;}

double ESR::GetXmax() const {return xrange_.second;}

/**
   return the path of input file
 */
std::string ESR::GetFilePath() const {return file_path_;};

/**
   return data length a.k.a. the size of vector
 */
int ESR::GetDataLength() const {return data_length_;};

/**
   return date string
 */
std::string ESR::GetDate() const {return date_;};

/**
   return date as Unix time.
 */
time_t ESR::GetDateAsUT() const
{
  tm tm_date;
  auto res = strptime(date_.c_str(), this->date_format.c_str(), &tm_date);

  if(not res) // null pointer case.
    {
      Warning("GetDateAsUT()", "Fail to parse \"%s\" as \"%s\"",
              date_.c_str(), date_format.c_str());
      return static_cast<time_t>(0);
    }
  else
    return mktime(&tm_date);
}

/**
   return gain a.k.a. normalisation factor
 */
double ESR::GetGain() const {return gain_;};

/**
   return shared pointer of graph
   Four kinds of graphs returned:
   is_norm | is_imag | returned graph
   -----|-----|-----
   0 | 0 | raw signal
   1 | 0 | normalised raw signal
   0 | 1 | imaginary part of raw signal
   1 | 1 | normalised imaginary part of raw signal

   0, 1: false, true, respectively.
 */
std::shared_ptr<TGraph> ESR::GetGraph(bool is_norm, bool is_imag) const
{
  // Here, internal counter of shared_ptr is incremented !
  if(is_imag)
    return is_norm? graph_imag_norm_ : graph_imag_;
  else
    return is_norm? graph_norm_ : graph_;
};

/**
   return shared pointer of graph.
   Similar to GetGraph function, graph pointer is specified by input parameters.

   @param is_norm normalised or not
   @param is_imag imaginary part or not
 */
std::shared_ptr<TGraph> ESR::GetGraphInteg(bool is_norm, bool is_imag) const
{
  if(is_imag)
    return is_norm? graph_imag_norm_integ_ : graph_imag_integ_;
  else
    return is_norm? graph_norm_integ_ : graph_integ_;
}

/**
   return a copy of vector x.
 */
std::vector<double> ESR::GetX() const {return vxdata_;};

/**
   return a copy of vector y. By changing inputs, four kinds of data returned.
 */
std::vector<double> ESR::GetY(bool is_norm, bool is_imag) const
{
  if(is_imag)
    return is_norm? vydata_norm_ : vydata_;
  else
    return is_norm? vydata_norm_integ_ : vydata_integ_;
};

/**
   print x and y range.
 */
void ESR::PrintRange() const
{
  std::cout << "Data range-------" << std::endl;
  std::cout << "x:from " << xrange_.first << "\tto\t" << xrange_.second
            << "\t(width: " << xrange_.second - xrange_.first << ")" << std::endl;
  std::cout << "y:from " << yrange_.first << "\tto\t" << yrange_.second
            << "\t(width: " << yrange_.second - yrange_.first << ")" << std::endl;
}

/**
   print some infomation.

   Inherits from TObject.
 */
void ESR::Print(Option_t*) const
{
  std::cout << "file path: " << file_path_ << std::endl
            << "data length: " << data_length_ << std::endl
            << "reduction factor: " << reduction_factor_ << std::endl
            << "gain: " << gain_ << std::endl;

  PrintRange();
}


/**
  write data into root file.
  Two TTrees are created: info and data.
  In the former, the raw header info are stored as pair<string, string>.
  In the letter, x, Re(y), Im(y), Re(normalised y), and Im(normalised y) are stored as double.

  For consistency of this file, the type of 'name' should be std::string.
  However, exceptionally const char* is used in order to keep consistency b/w TObject::Write.
*/
Int_t ESR::Write(const char* name, Int_t, Int_t) const
{
  auto f = std::shared_ptr<TFile>(TFile::Open(name, "recreate"));
  auto tree_header = new TTree("header", "header");
  auto tree_data = new TTree("data", "data");
  // `- do not use smart pointer for TTree...
  //    It is binded by TFile, which call dtor of all objects on the TFile directory when Close method is called.
  //    USE SMART POINTER FOR ONLY TFile.

  // header
  /* Accessing with pyROOT is no problem.*/
  std::pair<std::string, std::string> row;
  tree_header->Branch("info", &row);
  for(auto& pr : raw_header_)
    {
      row = pr;
      tree_header->Fill();
    }

  // data
  double x, y_real, y_imag, y_real_norm, y_imag_norm;
  tree_data->Branch("x", &x, "x/D");
  tree_data->Branch("y", &y_real, "y/D");
  tree_data->Branch("y_imag", &y_imag, "y_imag/D");
  tree_data->Branch("y_norm", &y_real_norm, "y_norm/D");
  tree_data->Branch("y_imag_norm", &y_imag_norm, "y_imag_norm/D");

  for(auto i = 0; i < data_length_; ++i)
    {
      x = vxdata_orig_.at(i);
      y_real = vydata_orig_.at(i);
      y_real_norm = vydata_norm_orig_.at(i);
      y_imag = vydata_imag_orig_.at(i);
      y_imag_norm = vydata_imag_norm_orig_.at(i);

      tree_data->Fill();
    }

  auto fsize = 0;
  fsize += tree_header->Write();
  fsize += tree_data->Write();
  f->Close();

  return fsize;
}
