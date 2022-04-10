/// \file GenTrainingDataException.cc
/// \brief Implementation of the GenTrainingDataException class

#include "training_data/GenTrainingDataException.hh"

#include <stdio.h>
#include <sstream>

using namespace GenTrainingDataErrorNum;

GenTrainingDataException::GenTrainingDataException(
    const std::string &_originMethod, ErrorNum _errorNumber, const std::string &arg1, const std::string &arg2)
    : Exception("GenTrainingData", "GenData", _originMethod, _errorNumber, arg1, arg2)
{
    InitErrorMessage();
}

void GenTrainingDataException::InitErrorMessage()
{
    const std::string vecName = GetArgument(0);
    switch(GetErrorNum())
    {
        case PARAMETER_LOAD_FAILURE:
            message = "Failed to load parameters for initialization.";
            break;
        case ROOT_FILE_OPEN_FAILURE:
            message = "Failed to open root file \"" + GetArgument(0) + "\'.";
            break;
        case ROOT_FILE_NOT_READY:
            message = "Root files are not ready to read.";
            break;
        case INPUT_SIZE_INCONSISTENT:
            message = "The size of input is not consistent with that in the configure file.";
            break;
        case OUTPUT_OUT_OF_RANGE:
            message = "Output data is out of range.";
            break;
        case EVENT_ID_MISMATCH:
            message = "Event IDs are not equal in the same event(" + GetArgument(0) + "," + GetArgument(1) + ").";
            break;
        case PAD_SIZES_MISMATCH:
            message = "The numbers of readout pads of event and background data do not coincide.";
        case OK:
            break;
        default:
            message = "Unidentified error number : " + GetErrorCode();
            break;
    }
}