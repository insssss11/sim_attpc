/// \file DetectorConstructionException.cc
/// \brief Implementation of the DetectorConstructionException class

#include "detector_construction/DetectorConstructionException.hh"

#include <stdio.h>
#include <sstream>

using namespace DetectorConstructionErrorNum;

DetectorConstructionException::DetectorConstructionException(
    const std::string &_originMethod, ErrorNum _errorNumber, const std::string &arg1, const std::string &arg2)
    : Exception("DetectorConstruction", "DetCon", _originMethod, _errorNumber, arg1, arg2)
{
    InitErrorMessage();
}

void DetectorConstructionException::InitErrorMessage()
{
    const std::string vecName = GetArgument(0);
    switch(GetErrorNum())
    {
        case INVALID_GAS_MATERIAL_NAME:
            message = "Cannot find material based on the Nist database : " + GetArgument(0);
            break;
        case OK:
            break;
        default:
            message = "Unidentified error number : " + GetErrorCode();
            break;
    }
}

G4ExceptionSeverity DetectorConstructionException::ClassifySeverity() const
{
    switch(GetErrorNum())
    {
        case INVALID_GAS_MATERIAL_NAME:
            return JustWarning;
        default:
            return JustWarning;
    }    
}