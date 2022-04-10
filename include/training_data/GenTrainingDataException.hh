/// \file GenTrainingDataException.hh
/// \brief Definition of the GenTrainingDataException class

#ifndef GenTrainingDataException_h
#define GenTrainingDataException_h 1

#include "exception/Exception.hh"

namespace GenTrainingDataErrorNum {
    enum ErrorNum :unsigned int {
        PARAMETER_LOAD_FAILURE,
        ROOT_FILE_OPEN_FAILURE,
        ROOT_FILE_NOT_READY,
        INPUT_SIZE_INCONSISTENT,
        OUTPUT_OUT_OF_RANGE,
        EVENT_ID_MISMATCH,
        PAD_SIZES_MISMATCH,
        OK = 9999
    };
}

class GenTrainingDataException : public Exception
{
    public:
    GenTrainingDataException(const std::string &originMethod,
        GenTrainingDataErrorNum::ErrorNum errorNumber,
        const std::string &arg1 = "", const std::string &arg2 = "");

    virtual void InitErrorMessage() override;
};
#endif
