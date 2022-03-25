/// \file ParamContainerTableException.cc
/// \brief Implementation of the ParamContainerTableException class

#include "config/ParamContainerTableException.hh"

using namespace ParamContainerTableErrorNum;

ParamContainerTableException::ParamContainerTableException(
    const std::string &_originMethod, ErrorNum _errorNumber,
    const std::string &argument1, const std::string &argument2)
    : Exception("ParamContainerTable", "ParamContainerTable", _originMethod, _errorNumber, argument1, argument2)
{
    InitErrorMessage();
}

void ParamContainerTableException::InitErrorMessage()
{
    const std::string argument1 = GetArgument(0), argument2 = GetArgument(1);
    switch(GetErrorNum())
    {
        case CONTAINER_DUPLICATED:
            message = "A parameter container with name \"" + argument1 + "\" aleady exists.";
            break;
        case CONTAINER_NOT_FOUND:
            message = "A parameter container \"" + argument1 + "\" does not exists.";
            break;
        case CONFIG_FILE_OPEN_FAILURE:
            message = "Failed to open the configuration file \"" + argument1 + "\".";
            break;
        case PARAM_FILE_OPEN_FAILURE:
            message = "Failed to open the parameter file \"" + argument1 + "\" with the name \"" + argument2 + "\".";
            break;
        case OK:
            break;
        default:
            message = "Unidentified error number : " + GetErrorCode();
            break;
    }
}