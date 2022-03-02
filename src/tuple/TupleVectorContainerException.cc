/// \file TupleVectorContainerException.cc
/// \brief Implementation of the TupleVectorContainerException class

#include "tuple/TupleVectorContainerException.hh"

#include <stdio.h>
#include <sstream>

using namespace TupleVectorContainerErrorNum;

TupleVectorContainerException::TupleVectorContainerException(
    const std::string &_originMethod, ErrorNum _errorNumber, const std::string &vecName)
    : Exception("TupleVectorContainer", "TupleVector", _originMethod, _errorNumber, vecName)
{
    InitErrorMessage();
}

void TupleVectorContainerException::InitErrorMessage()
{
    const std::string vecName = GetArgument(0);
    switch(GetErrorNum())
    {
        case VECTOR_DUPLICATED:
            message = "A vector with name \"" + vecName + "\" already exists.";
            break;
        case VECTOR_NOT_FOUND:
            message = "A vector \"" + vecName + "\" does not exist.";
            break;
        case OK:
            break;
        default:
            message = "Unidentified error number : " + GetErrorCode();
            break;
    }
}

G4ExceptionSeverity TupleVectorContainerException::ClassifySeverity() const
{
    switch(GetErrorNum())
    {
        case VECTOR_DUPLICATED:
            return JustWarning;
        case VECTOR_NOT_FOUND:
            return FatalException;
        default:
            return JustWarning;
    }    
}