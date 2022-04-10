/// \file TupleVectorManagerException.hh
/// \brief Definition of the TupleVectorManagerException class

#ifndef TupleVectorManagerException_h
#define TupleVectorManagerException_h 1

#include "exception/Exception.hh"

namespace TupleVectorManagerErrorNum {
    enum ErrorNum :unsigned int {
        TUPLE_DUPLICATED,
        TUPLE_NOT_FOUND,
        OK = 9999
    };
}

class TupleVectorManagerException : public Exception
{
    public:
    TupleVectorManagerException(const std::string &originMethod,
        TupleVectorManagerErrorNum::ErrorNum errorNumber,
        const std::string &tupleName);

    virtual void InitErrorMessage() override;
};

#endif
