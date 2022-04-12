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
        case MAGBOLTZ_UPDATE_FAILURE:
            message = "Failed to update the Magboltz instance with gas mixture : " + GetArgument(0);
            break;
        case G4MATERIAL_UPDATE_FAILURE:
            message = "Failed to update the G4Material instance with gas mixture : " + GetArgument(0);
            break;
        case GASFILE_DIR_NOT_FOUND:
            message = "Cannot find gas file directory at " + GetArgument(0);
            break;
        case GASFILE_LOAD_FAILURE:
            message = std::string("Cannot find corresponding gas file for electron drift speen, diffusion coeff and attachment.\n") +
                "Gas mixture is not updated.";
            break;
        case NOT_INITIALIZED:
            message = "Not initialized.";
            break;
        case MAGBOLTZ_CALCULATION_FAILURE:
            message = "Failed to estimate " + GetArgument(0);
            break;
        case OK:
            break;
        default:
            message = "Unidentified error number : " + GetErrorCode();
            break;
    }
}