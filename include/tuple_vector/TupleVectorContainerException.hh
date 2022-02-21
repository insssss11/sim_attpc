/// \file TupleVectorContainerException.hh
/// \brief Definition of the TupleVectorContainerException class

#ifndef TupleVectorContainerException_h
#define TupleVectorContainerException_h 1

#include "Exception.hh"

namespace TupleVectorContainerErrorNum {
    enum ErrorNum : unsigned int {
        VECTOR_DUPLICATED = 0000,
        VECTOR_NOT_FOUND = 0001,
        OK = 9999
    };
}

class TupleVectorContainerException : public Exception
{
    public:
    TupleVectorContainerException(const std::string &originMethod,
        TupleVectorContainerErrorNum::ErrorNum errorNumber,
        const std::string &vecName);

    virtual void InitErrorMessage() override;
    virtual G4ExceptionSeverity ClassifySeverity() const override;

    void SetVecName(const std::string &_vecName);

    std::string GetVecName() const;

    private:
    std::string vecName;
};

#endif
