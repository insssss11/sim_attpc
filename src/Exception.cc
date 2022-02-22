/// \file Exception.cc
/// \brief Implementation of the Exception class

#include "Exception.hh"

#include <iomanip>
#include <sstream>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Exception::~Exception()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const char *Exception::what() const noexcept
{
    return message.c_str();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Exception::WarnGeantKernel() const
{
    G4Exception(
        Where().c_str(),
        GetErrorCode().c_str(),
        ClassifySeverity(),
        what());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const std::string Exception::Where() const
{
    return originClass + "::" + originMethod;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const std::string Exception::GetOriginClass() const
{
    return originClass;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const std::string Exception::GetOriginMethod() const
{
    return originMethod;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const std::string Exception::GetErrorCode() const
{
    std::ostringstream os;
    os << errorPrefix << std::setfill('0') << std::setw(4) << errorNumber;
    return os.str();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const std::string Exception::GetErrorPrefix() const
{
    return errorPrefix;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

unsigned int Exception::GetErrorNum() const
{
    return errorNumber;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const std::string Exception::GetArgument(int idx) const
{
    return arguments.at(idx);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Exception::SetOriginMethod(const std::string &_originMethod)
{
    originMethod = _originMethod;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Exception::SetErrorNum(unsigned int _errorNumber)
{
    errorNumber = _errorNumber;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Exception::AddArgument(std::string argument)
{
    arguments.push_back(argument);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void Exception::AddArguments()
{
}