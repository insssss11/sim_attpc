/// \file Exception.hh
/// \brief Definition of the Exception class

#ifndef Exception_h
#define Exception_h 1

#include <vector>
#include <vector>
#include <exception>
#include <stdexcept>

#include "G4Exception.hh"
/// This exception class is wrapper of G4Exception, derived from standard exception class.
class Exception : public std::exception
{
    public:
    // When adding enum of error numbers for a derived class of this, must set 9999 as OK.
    template<typename... Strings>
    Exception(const std::string &originClass,
        const std::string &errorPrefix,
        const std::string &originMethod = "", unsigned int errorNum = 9999,
        Strings... strings);

    virtual ~Exception();

    // this must return error message after classification of error code.
    virtual const char *what() const noexcept override;
    virtual void InitErrorMessage() = 0; // This pure abstract method must be called the contstructor of a derived class.

    // this send exception to Geant4 by calling G4Exception
    virtual void WarnGeantKernel(G4ExceptionSeverity serverity) const;

    const std::string Where() const;
    const std::string GetOriginClass() const;
    const std::string GetOriginMethod() const;

    const std::string GetErrorCode() const;
    const std::string GetErrorPrefix() const;
    unsigned int GetErrorNum() const;
    const std::string GetArgument(int idx = 0) const;

    void SetOriginMethod(const std::string &method);
    void SetErrorNum(unsigned int errorNumber);

    protected:
    std::string message;

    private:
    void AddArgument(std::string argument);
    void AddArguments();
    template<typename String, typename... Strings>
    void AddArguments(String string, Strings... strings);

    private:
    const std::string originClass, errorPrefix;
    std::string originMethod;
    unsigned int errorNumber;
    std::vector<std::string> arguments;
};

template<typename... Strings>
Exception::Exception(const std::string &_originClass,
    const std::string &_errorPrefix,
    const std::string &_originMethod, unsigned int _errorNumber,
    Strings... strings)
    : std::exception(), originClass(_originClass), errorPrefix(_errorPrefix), originMethod(_originMethod), errorNumber(_errorNumber)
{
    arguments.reserve(sizeof...(strings));
    AddArguments(strings...);
}

template<typename String, typename... Strings>
void Exception::AddArguments(String string, Strings... strings)
{
    AddArgument(string);
    AddArguments(strings...);
}

#endif
