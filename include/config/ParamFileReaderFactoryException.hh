/// \file ParamFileReaderFactoryException.hh
/// \brief Definition of the ParamFileReaderFactoryException class

#ifndef ParamFileReaderFactoryException_h
#define ParamFileReaderFactoryException_h 1

#include "Exception.hh"

namespace ParamFileReaderFactoryErrorNum {
    enum ErrorNum : unsigned int {
        INVALID_READER_TYPE = 0000,
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
    virtual G4ExceptionSeverity ClassifySeverity() const override;

    void SetReaderType(const std::string &readerType);
    std::string GetReaderType() const;

    private:
    std::string readerType;
};

#endif
