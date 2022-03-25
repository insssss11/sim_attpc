/// \file DetectorConstructionException.hh
/// \brief Definition of the DetectorConstructionException class

#ifndef DetectorConstructionException_h
#define DetectorConstructionException_h 1

#include "Exception.hh"

namespace DetectorConstructionErrorNum {
    enum ErrorNum :unsigned int {
        INVALID_GAS_MATERIAL_NAME,
        OK = 9999
    };
}

class DetectorConstructionException : public Exception
{
    public:
    DetectorConstructionException(const std::string &originMethod,
        DetectorConstructionErrorNum::ErrorNum errorNumber,
        const std::string &arg1 = "", const std::string &arg2 = "");

    virtual void InitErrorMessage() override;
};
#endif
