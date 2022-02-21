/// \file Exception.hh
/// \brief Definition of the Exception class

#ifndef Exception_h
#define Exception_h 1

#include "G4Exception.hh"

#include <string>
#include <exception>

/// This exception class is wrapper of G4Exception, derived from standard exception class and .
class Exception : public std::exception
{
    public:
    Exception(const std::string &originClass,
        const std::string &errorPrefix,
        const std::string &originMethod = "", unsigned int errorNum = 9999);

    virtual ~Exception();
    
    // this must return error message after classification of error code.
    virtual const char *what() const noexcept override;
    virtual void InitErrorMessage() = 0; // this pure abstract method must be called the contstructor of a derived class.
    virtual G4ExceptionSeverity ClassifySeverity() const = 0;

    // this send exception to Geant4 by calling G4Exception
    void WarnGeantKernel() const;

    const std::string Where() const;
    const std::string GetOriginClass() const;
    const std::string GetOriginMethod() const;

    const std::string GetErrorCode() const;
    const std::string GetErrorPrefix() const;
    unsigned int GetErrorNum() const;

    void SetOriginMethod(const std::string &method);
    void SetErrorNum(unsigned int errorNumber);

    protected:
    std::string message;

    private:
    const std::string originClass, errorPrefix;
    std::string originMethod;
    unsigned int errorNumber;
};

#endif
