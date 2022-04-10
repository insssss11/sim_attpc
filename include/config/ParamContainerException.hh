/// \file ParamContainerException.hh
/// \brief Definition of the ParamContainerException class

#ifndef ParamContainerException_h
#define ParamContainerException_h 1

#include "exception/Exception.hh"

namespace ParamContainerErrorNum {
    enum ErrorNum : unsigned int {
        PARAM_DUPLICATED,
        PARAM_NOT_FOUND,
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
};

#endif
