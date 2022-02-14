/// \file Exception.hh
/// \brief Definition of the Exception class

#ifndef Exception_h
#define Exception_h 1

#include "G4Exception.hh"

#include <string>
#include <exception>

/// This exception class is derived from standard exception class and also call G4Exception when created.
class Exception : public std::exception
{
    public:
    Exception(const std::string &where, const std::string &code, G4ExceptionSeverity severity, const std::string &_message)
    : std::exception(), message(_message)
    {
        G4Exception(where.c_str(), code.c_str(), severity, _message.c_str());
    }
    virtual ~Exception() {}

    const char *what() const noexcept override
    {
        return message.c_str();
    }

    private:
    const std::string message;
};

#endif
