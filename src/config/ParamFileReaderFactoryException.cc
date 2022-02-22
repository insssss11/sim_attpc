/// \file ParamFileReaderFactoryException.cc
/// \brief Implementation of the ParamFileReaderFactoryException class

#include "config/ParamFileReaderFactoryException.hh"

using namespace ParamFileReaderFactoryErrorNum;

ParamFileReaderFactoryException::ParamFileReaderFactoryException(
    const std::string &_originMethod, ErrorNum _errorNumber, const std::string &_readerType)
    : Exception("ParamFileReaderFactory", "ParamFileReaderFactory", _originMethod, _errorNumber), readerType(_readerType)
{
    InitErrorMessage();
}

void ParamFileReaderFactoryException::InitErrorMessage()
{
    switch(GetErrorNum())
    {
        case INVALID_READER_TYPE:
            message = "Invalide argument \"" + readerType  + "\" as the type of ParamFileReader class.";
            break;
        case OK:
            break;
        default:
            message = "Unidentified error number : " + GetErrorCode();
            break;
    }
}

G4ExceptionSeverity ParamFileReaderFactoryException::ClassifySeverity() const
{
    switch(GetErrorNum())
    {
        case INVALID_READER_TYPE:
            return FatalException;
        default:
            return JustWarning;
    }    
}

void ParamFileReaderFactoryException::SetReaderType(const std::string &_readerType)
{
    readerType = _readerType;
}

std::string ParamFileReaderFactoryException::GetReaderType() const
{
    return readerType;
}