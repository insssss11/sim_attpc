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
    Exception(const std::string &_where, const std::string &_code, G4ExceptionSeverity _severity, const std::string &_message)
    : std::exception(), where(_where), code(_code), message(_message), severity(_severity)
    {
    }
    virtual ~Exception() {}

    const char *what() const noexcept override
    {
        return message.c_str();
    }
    void WarnGeantKernel() const
    {
        G4Exception(where.c_str(), code.c_str(), severity, message.c_str());
    }
    private:
    const std::string where, code, message;
    G4ExceptionSeverity severity;
};

#endif
