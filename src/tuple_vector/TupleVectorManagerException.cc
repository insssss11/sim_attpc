/// \file TupleVectorManagerException.cc
/// \brief Implementation of the TupleVectorManagerException class

#include "tuple_vector/TupleVectorManagerException.hh"

#include <stdio.h>
#include <sstream>

using namespace TupleVectorManagerErrorNum;

TupleVectorManagerException::TupleVectorManagerException(
    const std::string &_originMethod, ErrorNum _errorNumber, const std::string &_tupleName)
    : Exception("TupleVectorManager", "TupleVectorMan", _originMethod, _errorNumber), tupleName(_tupleName)
{
    InitErrorMessage();
}

void TupleVectorManagerException::InitErrorMessage()
{
    switch(GetErrorNum())
    {
        case TUPLE_DUPLICATED:
            message = "A tuple with name \"" + tupleName + "\" already exists.";
            break;
        case TUPLE_NOT_FOUND:
            message = "A tuple \"" + tupleName + "\" does not exist.";
            break;
        case OK:
            break;
        default:
            message = "Unidentified error number : " + GetErrorCode();
            break;
    }
}

G4ExceptionSeverity TupleVectorManagerException::ClassifySeverity() const
{
    switch(GetErrorNum())
    {
        case TUPLE_DUPLICATED:
            return JustWarning;
        case TUPLE_NOT_FOUND:
            return FatalException;
        default:
            return JustWarning;
    }    
}