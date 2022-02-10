/// \file TupleVectorContainerException.hh
/// \brief Definition of the TupleVectorContainerException class

#ifndef TupleVectorContainerException_h
#define TupleVectorContainerException_h 1

#include "G4Exception.hh"

#include <string>
#include <exception>

/// This exception class is used in TupleVectorcontainer throwing also G4Exception when called.
class TupleVectorContainerException : public std::exception
{
    public:
    TupleVectorContainerException(const std::string &_message = "") : message(_message) {}
    virtual ~TupleVectorContainerException() {}

    const char *what() const noexcept override
    {
        return message.c_str();
    }

    TupleVectorContainerException TupleNotFoundException(
        const std::string &where, const std::string &tName);
    TupleVectorContainerException TupleDuplicatedException(
        const std::string &where, const std::string &tName);
    TupleVectorContainerException VectorNotFoundException(
        const std::string &where, const std::string &tName, const std::string &vecName);
    TupleVectorContainerException VectorDuplicatedException(
        const std::string &where, const std::string &tName, const std::string &vecName);

    private:
    std::string message, whereAll;
};

#endif
