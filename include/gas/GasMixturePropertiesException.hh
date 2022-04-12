/// \file GasMixturePropertiesException.hh
/// \brief Definition of the GasMixturePropertiesException class

#ifndef GasMixturePropertiesException_h
#define GasMixturePropertiesException_h 1

#include "exception/Exception.hh"

namespace GasMixturePropertiesErrorNum {
    enum ErrorNum :unsigned int {
        MAGBOLTZ_UPDATE_FAILURE,
        G4MATERIAL_UPDATE_FAILURE,
        GASFILE_DIR_NOT_FOUND,
        GASFILE_LOAD_FAILURE,
        MAGBOLTZ_CALCULATION_FAILURE,
        NOT_INITIALIZED,
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
};
#endif
