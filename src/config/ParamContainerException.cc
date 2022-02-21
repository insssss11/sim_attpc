/// \file ParamContainerException.cc
/// \brief Implementation of the ParamContainerException class

#include "config/ParamContainerException.hh"

using namespace ParamContainerErrorNum;

ParamContainerException::ParamContainerException(
    const std::string &_originMethod, ErrorNum _errorNumber, const std::string &_paramName, const std::string &_paramType)
    : Exception("ParamContainer", "ParamContainer", _originMethod, _errorNumber), paramName(_paramName), paramType(_paramType)
{
    InitErrorMessage();
}

void ParamContainerException::InitErrorMessage()
{
    switch(GetErrorNum())
    {
        case PARAM_DUPLICATED:
            message = "A " + paramType + " type parameter with name \"" + paramName + "\" already exists.";
            break;
        case PARAM_NOT_FOUND:
            message = "A " + paramType + " type parameter \"" + paramName + "\" does not exist.";
            break;
        case OK:
            break;
        default:
            message = "Unidentified error number : " + GetErrorCode();
            break;
    }
}

G4ExceptionSeverity ParamContainerException::ClassifySeverity() const
{
    switch(GetErrorNum())
    {
        case PARAM_DUPLICATED:
            return JustWarning;
        case PARAM_NOT_FOUND:
            return FatalException;
        default:
            return JustWarning;
    }    
}

void ParamContainerException::SetParamName(const std::string &_paramName)
{ 
    paramName = _paramName;
}

std::string ParamContainerException::GetParamName() const
{
    return paramName;
}