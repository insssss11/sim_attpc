/// \file TupleVectorContainerException.cc
/// \brief Implementation of the TupleVectorContainerException class

#include "tuple_vector/TupleVectorContainerException.hh"

#include <stdio.h>
#include <sstream>

using namespace TupleVectorContainerErrorNum;

TupleVectorContainerException::TupleVectorContainerException(
    const std::string &_originMethod, ErrorNum _errorNumber, const std::string &_vecName)
    : Exception("TupleVectorContainer", "TupleVector", _originMethod, _errorNumber), vecName(_vecName)
{
    InitErrorMessage();
}

void TupleVectorContainerException::InitErrorMessage()
{
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

void TupleVectorContainerException::SetVecName(const std::string &_vecName)
{
    vecName = _vecName;
}

std::string TupleVectorContainerException::GetVecName() const
{
    return vecName;
}