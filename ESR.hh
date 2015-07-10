#ifndef ESR_hh
#define ESR_hh


#include <iostream>
#include <vector>
#include <utility>
#include <memory>
#include <map>

#include "TObject.h"


// forward declaration
class TGraph;
class TTree;
class ESRHeader;

// typedef: useless...
using pgraph = std::shared_ptr<TGraph>;


struct GraphStyle
{
  GraphStyle();
  GraphStyle(int, int);
  GraphStyle(const std::tuple<int, int, int, int, int, int>&);
  int line_style;// = 0;
  int line_color;// = 0;
  int line_width;// = 0;
  int marker_style;// = 20;
  int marker_color;// = 2;
  int marker_size;// = 1;
};


class ESR : public TObject
{
  int file_type_;
  std::shared_ptr<ESRHeader> esr_header_;

  std::map<std::string, std::string> raw_header_;
  std::string file_path_;
  int data_length_;
  int reduction_factor_;
  std::pair<double, double> xrange_;
  std::pair<double, double> yrange_;
  std::string date_;
  double gain_; // amplitude(fine) * 10^(amplitude(course) )

  // real part
  std::vector<double> vxdata_;
  std::vector<double> vydata_;
  std::vector<double> vydata_integ_;
  std::vector<double> vydata_norm_;
  std::vector<double> vydata_norm_integ_;

  // imaginary part
  std::vector<double> vydata_imag_;
  std::vector<double> vydata_imag_integ_;
  std::vector<double> vydata_imag_norm_;
  std::vector<double> vydata_imag_norm_integ_;

  /* original data. data to be processed further are "reduced" data.
     original one are stored and can be accessed.
   */
  std::vector<double> vxdata_orig_;
  std::vector<double> vydata_orig_;
  std::vector<double> vydata_norm_orig_;
  std::vector<double> vydata_imag_orig_;
  std::vector<double> vydata_imag_norm_orig_;

  // stop nama-po
  std::shared_ptr<TGraph> graph_;
  std::shared_ptr<TGraph> graph_norm_;
  std::shared_ptr<TGraph> graph_integ_;
  std::shared_ptr<TGraph> graph_norm_integ_;
  std::shared_ptr<TGraph> graph_imag_;
  std::shared_ptr<TGraph> graph_imag_norm_;
  std::shared_ptr<TGraph> graph_imag_integ_;
  std::shared_ptr<TGraph> graph_imag_norm_integ_;

  // methods
  bool CheckInputFile(std::ifstream&);
  void CheckReductionFactor();
  void CheckFileFormatType(std::ifstream&);

  // helper function
  std::string trim(const std::string&);
  std::string replace(const std::string& s, const char rep = '~', const char to = ' ');

  // functions
  std::map<std::string, std::string> ParseHeader(std::ifstream&);
  void MakeHeader(const std::map<std::string, std::string>&);
  void ParseData(std::ifstream&);
  void LoadFromROOTFile();

  void SetParams();
  void ReduceData();

  std::pair<std::vector<double>, std::vector<double> >
  DoIntegral(const std::vector<double>&, const std::vector<double>&, double, double,
             const std::pair<bool, double> integral_constant = {false, 0}) const;
  void IntegrateData();

  void MakeAllGraphs();
  TGraph* MakeGraph(const std::vector<double>&, const std::vector<double>&) const;
  void MakeupGraph(const std::shared_ptr<TGraph>&,
                   const GraphStyle&, const std::string title = "") const;

public:
  ESR();
  ESR(const std::string, const int reduction_factor = 1);
  ESR& operator=(const ESR&); // copy operator=
  ESR(const ESR&); // copy constructor
  ESR& operator=(ESR&& esr) noexcept; // move operator=
  ESR(ESR&&) noexcept; // move constructor
  ~ESR();

  // static member
  static std::string x_axis_title;// = "Magnetic field (mT)";
  static std::string y_axis_title;// = "Amplitude"
  static GraphStyle gs_sig;
  static GraphStyle gs_sig_integ;
  static GraphStyle gs_sig_imag;
  static GraphStyle gs_sig_imag_integ;
  static std::string date_format; // "%Y/%m/%d %H:%M"   ~ is removed.

  // --- methods ---
  /*  getter  */
  int GetFileType() const;
  std::shared_ptr<ESRHeader> GetHeader() const;
  std::string GetFilePath() const;
  int GetDataLength() const;
  std::string GetDate() const;
  time_t GetDateAsUT() const; // muda-function
  double GetGain() const;

  std::shared_ptr<TGraph> GetGraph(bool is_norm = false, bool is_imag = false) const;
  std::shared_ptr<TGraph> GetGraphInteg(bool is_norm = false, bool is_imag = false) const;
  std::shared_ptr<TGraph> GetGraphIntegPart(double, double,
                                            const std::pair<bool, double> integral_constant = {false, 0},
                                            bool is_norm = false, bool is_imag = false) const;

  std::pair<double, double> GetXrange() const; // just return header info
  std::pair<double, double> GetYrange() const; // just return header info
  double GetXmin() const;
  double GetXmax() const;
  std::vector<double> GetX() const;
  std::vector<double> GetY(bool is_norm = false, bool is_imag = false) const;

  // setter
  void SetReductionFactor(int reduction_factor = 1);

  // function
  double Integrate(double, double, bool is_norm = false, bool is_imag = false,
                   const std::pair<bool, double> integral_constant = {false, 0}) const;
  //   virtual void Write(const std::string output = "esr.root") const;
  virtual Int_t Write(const char* name=nullptr, Int_t option=0, Int_t bufsize=0) const final;

  // utility
  void PrintRange() const;
  virtual void Print(Option_t* options = "") const final;


  ClassDef(ESR, 1);
};


// // load namspace
// #include "ESRUtil.hxx"


#endif
