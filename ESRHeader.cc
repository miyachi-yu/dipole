#include "ESRHeader.hh"
#include "ESRHeaderElement.hh"

#include "TError.h"

ClassImp(ESRHeader)


ESRHeader::ESRHeader() :
  header_dh_{std::make_shared<ESRHeaderDH>()},
  header_gp_{std::make_shared<ESRHeaderGP>()},
  header_sp_{std::make_shared<ESRHeaderSP>()},
  header_ap_{std::make_shared<ESRHeaderAP>()},
  header_dt_{std::make_shared<ESRHeaderDT>()}
{};

ESRHeader::ESRHeader(const std::map<std::string, std::string>& key_val)
  : header_dh_(std::make_shared<ESRHeaderDH>(key_val)),
    header_gp_(std::make_shared<ESRHeaderGP>(key_val)),
    header_sp_(std::make_shared<ESRHeaderSP>(key_val)),
    header_ap_(std::make_shared<ESRHeaderAP>(key_val)),
    header_dt_(std::make_shared<ESRHeaderDT>(key_val))
{}

ESRHeader::~ESRHeader()
{
  header_dh_.reset();
  header_gp_.reset();
  header_sp_.reset();
  header_ap_.reset();
  header_dt_.reset();
}

// static method
int ESRHeader::CheckFileFormatType(std::ifstream& ifs)
{
    /*
    ## GOOD file
    $ cat data.txt
    ===== Data Head ===================================
    file name .....

    ## BAD file
    $ cat data.txt
    waves=1 length=65252 data=CH1/2
    .....

    ## file type
    0 : normal file
    1 : wave format file
    2 : other file
   */
  ifs.seekg(ifs.beg);
  std::string buf;
  std::getline(ifs, buf);
  ifs.seekg(ifs.beg);

  if(buf.find("Data Head") != buf.npos)
    return 0;
  else if(buf.find("wave") != buf.npos)
    {
      ESRHeader{}.Info("CheckFileFormatType", "wave file format");
      return 1;
    }
  else
    {
      ESRHeader{}.Warning("CheckFileFormatType", "Strange file format");
      return 2;
    }
}

// getter. return copy
std::shared_ptr<ESRHeaderDH> ESRHeader::GetDataHead() const {return header_dh_;}

std::shared_ptr<ESRHeaderGP> ESRHeader::GetGeneralParameter() const { return header_gp_;}

std::shared_ptr<ESRHeaderSP> ESRHeader::GetSpectometerParameter() const {return header_sp_;}

std::shared_ptr<ESRHeaderAP> ESRHeader::GetAcquisitionParameter() const {return header_ap_;}

std::shared_ptr<ESRHeaderDT> ESRHeader::GetEsrData() const {return header_dt_;}

int ESRHeader::GetDataLength() const {return header_dh_->GetDataLength();}

std::string ESRHeader::GetDate() const {return header_gp_->GetDate();}

std::pair<double, double> ESRHeader::GetXrange() const {return header_dh_->GetXrange();}

std::pair<double, double> ESRHeader::GetYrange() const {return header_dh_->GetYrange();}

std::pair<double, double> ESRHeader::GetAmplitude(int type) const {return header_sp_->GetAmplitude(type);}

// setter
void ESRHeader::SetXrange(std::pair<double, double> val)
{
  header_dh_->SetXrange(val.first);
  header_dh_->SetXrangeMin(val.second - val.first);
}

void ESRHeader::SetAmplitude(std::pair<double, double> val, int type)
{
  header_sp_->SetAmplitude(std::forward<std::pair<double, double>>(val),
                           std::forward<int>(type));
}

// print
void ESRHeader::Print(Option_t* options) const
{
  header_dh_->Print(options);
  header_gp_->Print(options);
  header_sp_->Print(options);
  header_ap_->Print(options);
  header_dt_->Print(options);
}
