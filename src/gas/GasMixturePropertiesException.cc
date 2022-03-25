/// \file GasMixturePropertiesException.cc
/// \brief Implementation of the GasMixturePropertiesException class

#include "gas/GasMixturePropertiesException.hh"

#include <stdio.h>
#include <sstream>

using namespace GasMixturePropertiesErrorNum;

GasMixturePropertiesException::GasMixturePropertiesException(
    const std::string &_originMethod, ErrorNum _errorNumber, const std::string &arg1, const std::string &arg2)
    : Exception("GasMixtureProperties", "MediumMagboltz", _originMethod, _errorNumber, arg1, arg2)
{
    InitErrorMessage();
}

void GasMixturePropertiesException::InitErrorMessage()
{
    const std::string vecName = GetArgument(0);
    switch(GetErrorNum())
    {
        case INVALID_GAS_COMPONENT_NAME:
            message = "Failed to initialize a gas mixture : " + GetArgument(0);
            break;
        case NOT_INITIALIZED:
            message = "Gas composition not initialized.";
            break;
        case MAGBOLTZ_CALCULATION_FAILURE:
            message = "Magboltz failed to calculate " + GetArgument(0) + ".";
        case OK:
            break;
        default:
            message = "Unidentified error number : " + GetErrorCode();
            break;
    }
}