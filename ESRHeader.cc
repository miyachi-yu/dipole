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


// getter. return copy
std::shared_ptr<ESRHeaderDH> ESRHeader::GetDataHead() const {return header_dh_;}

std::shared_ptr<ESRHeaderGP> ESRHeader::GetGeneralParameter() const { return header_gp_;}

std::shared_ptr<ESRHeaderSP> ESRHeader::GetSpectrometerParameter() const {return header_sp_;}

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
  // little bit overcost. Passing copies is more simple way.
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
