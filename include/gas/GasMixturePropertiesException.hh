/// \file GasMixturePropertiesException.hh
/// \brief Definition of the GasMixturePropertiesException class

#ifndef GasMixturePropertiesException_h
#define GasMixturePropertiesException_h 1

#include "Exception.hh"

namespace GasMixturePropertiesErrorNum {
    enum ErrorNum :unsigned int {
        INVALID_GAS_COMPONENT_NAME,
        NOT_INITIALIZED,
        MAGBOLTZ_CALCULATION_FAILURE,
        OK = 9999
    };
}

class GasMixturePropertiesException : public Exception
{
    public:
    GasMixturePropertiesException(const std::string &originMethod,
        GasMixturePropertiesErrorNum::ErrorNum errorNumber,
        const std::string &arg1 = "", const std::string &arg2 = "");

    virtual void InitErrorMessage() override;
    virtual G4ExceptionSeverity ClassifySeverity() const override;
};
#endif
