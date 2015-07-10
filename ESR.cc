#include "ESR.hh"
#include "ESRHeader.hh"
#include "ESRHeaderElement.hh"
// #include "ESRUtil.hxx"

#include <fstream>
#include <stdexcept>
#include <iomanip>
#include <algorithm>
#include <time.h>

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


/**
   詳細はREADME.md．
 */


// static variables
std::string ESR::date_format = "%Y/%m/%d %H:%M"; //   ~ is removed.
std::string ESR::x_axis_title = "Magnetic field (mT)";
std::string ESR::y_axis_title = "Amplitude";
GraphStyle ESR::gs_sig = {64, 2};
GraphStyle ESR::gs_sig_integ = {2, 3};
GraphStyle ESR::gs_sig_imag = {412, 1};
GraphStyle ESR::gs_sig_imag_integ = {807, 2};



// constructors
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
ESR::ESR(std::string file_path, int reduction_factor) :
  file_type_(-1), file_path_(file_path), data_length_(0), reduction_factor_(reduction_factor),
  xrange_{0, 0}, yrange_{0, 0}, date_(""), gain_(0),
  graph_(nullptr), graph_norm_(nullptr), graph_integ_(nullptr), graph_norm_integ_(nullptr),
  graph_imag_(nullptr), graph_imag_norm_(nullptr),
  graph_imag_integ_(nullptr), graph_imag_norm_integ_(nullptr)
{
  Info("ESR(std::string, int)", "cnstr");

  std::ifstream ifs(file_path_.c_str());
  if(not CheckInputFile(ifs))
    return;

  CheckFileFormatType(ifs);

  if(file_type_ < 2)
    {
      ifs.seekg(ifs.beg);
      raw_header_ = std::move(ParseHeader(ifs));
      MakeHeader(raw_header_);
      SetParams();
      ParseData(ifs);
      ifs.close();
    }
  else if(file_type_ == 2)
    {
      ifs.close();
      LoadFromROOTFile();
    }

  CheckReductionFactor();
  ReduceData();
  IntegrateData();
  MakeAllGraphs();
}

// copy constructor
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

ESR::ESR(const ESR& esr)
{
  Info("ESR(const ESR&)", "operator=(const ESR&) is called.");
  *this = esr;
}

// http://qiita.com/termoshtt/items/3397c149bf2e4ce07e6c#move-assignment-operator-%E7%A7%BB%E5%8B%95%E4%BB%A3%E5%85%A5%E6%BC%94%E7%AE%97%E5%AD%90
// move
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

ESR::ESR(ESR&& esr) noexcept
{
  Info("ESR(ESR&&)", "operator=(ESR&&) is called.");
  *this = std::move(esr);
}

ESR::~ESR()
{
  Info("~ESR()", "delete cnstr");

  // automatically destorated.
  // // reset! reset!! reset!!!
  // std::cout << "is unique? " << graph_.unique() << std::endl;
  // graph_.reset();
  // graph_norm_.reset();
  // graph_integ_.reset();
  // graph_norm_integ_.reset();
  // graph_imag_.reset();
  // graph_imag_norm_.reset();
  // graph_imag_integ_.reset();
  // graph_imag_norm_integ_.reset();
}

// helper function
std::string ESR::trim(const std::string& s)
{
  // http://www.toptip.ca/2010/03/trim-leading-or-trailing-white-spaces.html
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
      for(auto i = 0; i < 77; ++i)
        {
          std::getline(ifs, buf);
          auto ind = buf.find("=");
          if(ind == 0)
            continue;

          header_key_val[this->trim(this->replace(buf.substr(0, ind)))]
            = this->trim(this->replace(buf.substr(ind + 1)));
        }
    }
  else if(file_type_ == 1)
    {
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
  else
    {
      // never happen.
    }

  return std::move(header_key_val);
}

void ESR::MakeHeader(const std::map<std::string, std::string>& key_val)
{
  esr_header_ = std::make_shared<ESRHeader>(key_val);
}

void ESR::ParseData(std::ifstream& ifs)
{
  if(file_type_ < 0 or file_type_ > 1)
    {
      Fatal("", "FUCK. you specify 0 or 1 for normal file or wave format file");
      throw std::runtime_error("fuck fuck fuck");
    }

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
          val = std::stod(buf);

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
          val = std::stod(buf);

          vydata_imag_orig_.push_back(val);
          vydata_imag_norm_orig_.push_back(val / gain_);
        }
    }
  else
    {
      // wave format
      std::string buf;
      std::getline(ifs, buf);
      std::getline(ifs, buf);

      // reserve buffer of vectors
      vxdata_orig_.reserve(data_length_);
      vydata_orig_.reserve(data_length_);
      vydata_norm_orig_.reserve(data_length_);

      auto val = double{0};
      for(auto il = 0; il < data_length_; ++il)
        {
          ifs >> buf;
          val = std::stod(buf);
          vxdata_orig_.push_back(val);

          ifs >> buf;
          val = std::stod(buf);
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
          ifs >> buf;
          ifs >> buf;
          val = std::stod(buf);
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
}

void ESR::LoadFromROOTFile()
{

  auto deleter = [](TTree* p) -> void
    {
      // std::cout << "custom deleter called for " << p->GetName() << std::endl;
      // std::cout << "on heap? " << std::boolalpha << p->IsOnHeap() << std::endl;
      if(p->IsOnHeap())
        delete p;
      // delete p;
      // p = nullptr;
      // std::cout << "EO custom deleter" << std::endl;
    };

  auto tf = std::unique_ptr<TFile>(TFile::Open(file_path_.c_str()));
  auto tree_header = std::unique_ptr<TTree, void(*)(TTree*)>(dynamic_cast<TTree*>(tf->Get("header")), deleter);
  auto tree_data = std::unique_ptr<TTree, void(*)(TTree*)>(dynamic_cast<TTree*>(tf->Get("data")), deleter);
  // auto* tf = TFile::Open(file_path_.c_str());
  // auto* tree_header = dynamic_cast<TTree*>(tf->Get("header"));
  // auto* tree_data = dynamic_cast<TTree*>(tf->Get("data"));

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

void ESR::SetParams()
{
  data_length_ = esr_header_->GetDataLength();
  date_ = esr_header_->GetDate();

  xrange_ = esr_header_->GetXrange();
  yrange_ = esr_header_->GetYrange();

  auto amp1s = esr_header_->GetAmplitude(1);
  gain_ = amp1s.first * std::pow(10, amp1s.second);
}

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

bool ESR::CheckInputFile(std::ifstream& ifs)
{
  if(ifs.fail())
    {
      Error("CheckInputFile", "File open error");
      return false;
    }
  return true;
}

void ESR::CheckFileFormatType(std::ifstream& ifs)
{
  /*
    ## GOOD file
    $ cat data.txt
    ===== Data Head ===================================
    file name .....

    ## waveformat file
    $ cat data.txt
    waves=1 length=65252 data=CH1/2
    .....

    ## file type
   -1 : other file
    0 : normal file
    1 : wave format file
    2 : ROOT file
   */

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
  char cbuf[4];
  ifs.read(cbuf, 4); // root
  auto header_id = std::string(cbuf, 4);
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

  if(header_id == "root" and (root_version > 50000))
    {
      Info("ESR::CheckFileFormatType", "root file");
      file_type_ = 2;
      return;
    }
  else
    {
      Warning("CheckFileFormatType", "Strange file format");
      file_type_ = -1;
      return;
    }

  // never happen
  return;
}


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

TGraph* ESR::MakeGraph(const std::vector<double>& x,
                              const std::vector<double>& y) const
{
  return new TGraph(x.size(), x.data(), y.data());
}

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


void ESR::SetReductionFactor(int reduction_factor)
{
  reduction_factor_ = reduction_factor;
  CheckReductionFactor();

  // recalculate data
  ReduceData();
  IntegrateData();
  MakeAllGraphs();
}

double ESR::Integrate(double start, double end, bool is_norm, bool is_imag,
                      const std::pair<bool, double> integral_constant) const
{
  const std::vector<double>* pvy;
  if(is_imag)
    pvy = is_norm? &vydata_imag_norm_ : &vydata_imag_;
  else
    pvy = is_norm? &vydata_norm_ : &vydata_;

  const auto vxy = std::move(this->DoIntegral(vxdata_, *pvy, start, end, integral_constant));
  return std::accumulate(vxy.second.cbegin(), vxy.second.cend(), double{0});
}



/*
  getter
 */
int ESR::GetFileType() const {return file_type_;};

std::shared_ptr<ESRHeader> ESR::GetHeader() const {return esr_header_;}

std::pair<double, double> ESR::GetXrange() const {return esr_header_->GetXrange();}

std::pair<double, double> ESR::GetYrange() const {return esr_header_->GetYrange();}

double ESR::GetXmin() const {return xrange_.first;}

double ESR::GetXmax() const {return xrange_.second;}

std::string ESR::GetFilePath() const {return file_path_;};

int ESR::GetDataLength() const {return data_length_;};

std::string ESR::GetDate() const {return date_;};

time_t ESR::GetDateAsUT() const
{
  tm tm_date;
  auto res = strptime(date_.c_str(), this->date_format.c_str(), &tm_date);

  if(not res) // null pointer case.
    {
      Warning("GetDateAsUT()", "Fail to parse \"%s\" as \"%s\"",
              date_.c_str(), date_format.c_str());
      return 0ul;
    }
  else
    return mktime(&tm_date);
}

double ESR::GetGain() const {return gain_;};

std::shared_ptr<TGraph> ESR::GetGraph(bool is_norm, bool is_imag) const
{
  // Here, internal counter of shared_ptr is incremented !
  if(is_imag)
    return is_norm? graph_imag_norm_ : graph_imag_;
  else
    return is_norm? graph_norm_ : graph_;
};

std::shared_ptr<TGraph> ESR::GetGraphInteg(bool is_norm, bool is_imag) const
{
  if(is_imag)
    return is_norm? graph_imag_norm_integ_ : graph_imag_integ_;
  else
    return is_norm? graph_norm_integ_ : graph_integ_;
}

std::vector<double> ESR::GetX() const {return vxdata_;};

std::vector<double> ESR::GetY(bool is_norm, bool is_imag) const
{
  if(is_imag)
    return is_norm? vydata_norm_ : vydata_;
  else
    return is_norm? vydata_norm_integ_ : vydata_integ_;
};


/*
  print function
 */
void ESR::PrintRange() const
{
  std::cout << "Data range-------" << std::endl;
  std::cout << "x:from " << xrange_.first << "\tto\t" << xrange_.second
            << "\t(width: " << xrange_.second - xrange_.first << ")" << std::endl;
  std::cout << "y:from " << yrange_.first << "\tto\t" << yrange_.second
            << "\t(width: " << yrange_.second - yrange_.first << ")" << std::endl;
}

void ESR::Print(Option_t*) const
{
  std::cout << "file path: " << file_path_ << std::endl
            << "data length: " << data_length_ << std::endl
            << "reduction factor: " << reduction_factor_ << std::endl
            << "gain: " << gain_ << std::endl;

  PrintRange();
}


/*
  write data into root file.
*/
// void ESR::Write(const std::string output) const
Int_t ESR::Write(const char* name, Int_t, Int_t) const
{
  auto f = std::shared_ptr<TFile>(TFile::Open(name, "recreate"));
  auto tree_header = new TTree("header", "header");
  auto tree_data = new TTree("data", "data");
  // `- do not use smart pointer for TTree...
  //    It is binded by TFile, which call dtor of all objects on the TFile directory when Close method is called.
  //    ONLY USE SMART POINTER FOR TFile.

  // header
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
