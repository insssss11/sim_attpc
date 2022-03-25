/// \file ParamContainerException.cc
/// \brief Implementation of the ParamContainerException class

#include "config/ParamContainerException.hh"

using namespace ParamContainerErrorNum;

ParamContainerException::ParamContainerException(
    const std::string &_originMethod, ErrorNum _errorNumber, const std::string &paramName, const std::string &paramType)
    : Exception("ParamContainer", "ParamContainer", _originMethod, _errorNumber, paramName, paramType)
{
    InitErrorMessage();
}

void ParamContainerException::InitErrorMessage()
{
    const std::string paramType = GetArgument(0), paramName = GetArgument(1);
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