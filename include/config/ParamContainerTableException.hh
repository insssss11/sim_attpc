/// \file ParamContainerTableException.hh
/// \brief Definition of the ParamContainerTableException class

#ifndef ParamContainerTableException_h
#define ParamContainerTableException_h 1

#include "Exception.hh"

namespace ParamContainerTableErrorNum {
    enum ErrorNum : unsigned int {
        CONTAINER_DUPLICATED,
        CONTAINER_NOT_FOUND,
        CONFIG_FILE_OPEN_FAILURE,
        PARAM_FILE_OPEN_FAILURE,
        OK = 9999
    };
}

class ParamContainerTableException : public Exception
{
    public:
    ParamContainerTableException(const std::string &originMethod,
        ParamContainerTableErrorNum::ErrorNum errorNumber,
        const std::string &argument1, const std::string &argument2 = "");

    virtual void InitErrorMessage() override;
};

#endif
