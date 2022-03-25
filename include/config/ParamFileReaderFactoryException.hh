/// \file ParamFileReaderFactoryException.hh
/// \brief Definition of the ParamFileReaderFactoryException class

#ifndef ParamFileReaderFactoryException_h
#define ParamFileReaderFactoryException_h 1

#include "Exception.hh"

namespace ParamFileReaderFactoryErrorNum {
    enum ErrorNum : unsigned int {
        INVALID_READER_TYPE,
        OK = 9999
    };
}

class ParamFileReaderFactoryException : public Exception
{
    public:
    ParamFileReaderFactoryException(const std::string &originMethod,
        ParamFileReaderFactoryErrorNum::ErrorNum errorNumber,
        const std::string &readerType);

    virtual void InitErrorMessage() override;
};

#endif
