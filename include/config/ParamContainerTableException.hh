/// \file ParamContainerTableException.hh
/// \brief Definition of the ParamContainerTableException class

#ifndef ParamContainerTableException_h
#define ParamContainerTableException_h 1

#include "Exception.hh"

namespace ParamContainerTableErrorNum {
    enum ErrorNum : unsigned int {
        CONTAINER_DUPLICATED = 0000,
        CONTAINER_NOT_FOUND = 0001,
        NOT_INITIALIZED = 0004,
        OK = 9999
    };
}

class ParamContainerTableException : public Exception
{
    public:
    ParamContainerTableException(const std::string &originMethod,
        ParamContainerTableErrorNum::ErrorNum errorNumber,
        const std::string &containerName = "");

    virtual void InitErrorMessage() override;
    virtual G4ExceptionSeverity ClassifySeverity() const override;

    void SetContainerName(const std::string &containerName);
    std::string GetContainerName() const;

    private:
    std::string containerName;
};

#endif
