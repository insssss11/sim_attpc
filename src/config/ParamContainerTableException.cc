/// \file ParamContainerTableException.cc
/// \brief Implementation of the ParamContainerTableException class

#include "config/ParamContainerTableException.hh"

using namespace ParamContainerTableErrorNum;

ParamContainerTableException::ParamContainerTableException(
    const std::string &_originMethod, ErrorNum _errorNumber, const std::string &_containerName)
    : Exception("ParamContainerTable", "ParamContainerTable", _originMethod, _errorNumber), containerName(_containerName)
{
    InitErrorMessage();
}

void ParamContainerTableException::InitErrorMessage()
{
    switch(GetErrorNum())
    {
        case CONTAINER_DUPLICATED:
            message = "A parameter container with name \"" + containerName + "\" aleady exists.";
            break;
        case CONTAINER_NOT_FOUND:
            message = "A parameter container \"" + containerName + "\" does not exists.";
            break;
        case NOT_INITIALIZED:
            message = "The parameter container table is empty. Initialize loading configuration file.";
            break;
        case OK:
            break;
        default:
            message = "Unidentified error number : " + GetErrorCode();
            break;
    }
}

G4ExceptionSeverity ParamContainerTableException::ClassifySeverity() const
{
    switch(GetErrorNum())
    {
        case CONTAINER_DUPLICATED:
            return JustWarning;
        case CONTAINER_NOT_FOUND:
            return FatalException;
        case NOT_INITIALIZED:
            return FatalException;
        default:
            return JustWarning;
    }    
}

void ParamContainerTableException::SetContainerName(const std::string &_containerName)
{ 
    containerName = _containerName;
}

std::string ParamContainerTableException::GetContainerName() const
{
    return containerName;
}