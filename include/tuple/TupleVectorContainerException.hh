/// \file TupleVectorContainerException.hh
/// \brief Definition of the TupleVectorContainerException class

#ifndef TupleVectorContainerException_h
#define TupleVectorContainerException_h 1

#include "Exception.hh"

namespace TupleVectorContainerErrorNum {
    enum ErrorNum : unsigned int {
        VECTOR_DUPLICATED,
        VECTOR_NOT_FOUND,
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
};

#endif
