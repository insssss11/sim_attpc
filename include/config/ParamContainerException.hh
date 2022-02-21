/// \file ParamContainerException.hh
/// \brief Definition of the ParamContainerException class

#ifndef ParamContainerException_h
#define ParamContainerException_h 1

#include "Exception.hh"

namespace ParamContainerErrorNum {
    enum ErrorNum : unsigned int {
        PARAM_DUPLICATED = 0000,
        PARAM_NOT_FOUND = 0001,
        OK = 9999
    };
}

class ParamContainerException : public Exception
{
    public:
    ParamContainerException(const std::string &originMethod,
        ParamContainerErrorNum::ErrorNum errorNumber,
        const std::string &paramName, const std::string &_type);

    virtual void InitErrorMessage() override;
    virtual G4ExceptionSeverity ClassifySeverity() const override;

    void SetParamName(const std::string &_vecName);

    std::string GetParamName() const;

    private:
    std::string paramName, paramType;
};

#endif
