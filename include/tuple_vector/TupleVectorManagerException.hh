/// \file TupleVectorManagerException.hh
/// \brief Definition of the TupleVectorManagerException class

#ifndef TupleVectorManagerException_h
#define TupleVectorManagerException_h 1

#include "Exception.hh"

namespace TupleVectorManagerErrorNum {
    enum ErrorNum :unsigned int {
        TUPLE_DUPLICATED = 0000,
        TUPLE_NOT_FOUND = 0001,
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
    virtual G4ExceptionSeverity ClassifySeverity() const override;

    void SetTupleName(const std::string &_tupleName);

    std::string GetTupleName() const;

    private:
    std::string tupleName;
};

#endif
