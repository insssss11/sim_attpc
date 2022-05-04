/// \file TreeMergerException.hh
/// \brief Definition of the TreeMergerException class

#ifndef TreeMergerException_h
#define TreeMergerException_h 1

#include "exception/Exception.hh"

namespace TreeMergerErrorNum {
    enum ErrorNum : unsigned int {
        THREAD_FILE_OPEN_FAILURE,
        THREAD_FILE_REMOVE_FAILURE,
        TREE_NOT_FOUND,
        MERGED_TREE_EMPTY,
        OK = 9999
    };
}

class TreeMergerException : public Exception
{
    public:
    TreeMergerException(const std::string &originMethod,
        TreeMergerErrorNum::ErrorNum errorNumber,
        const std::string &arg1 = "", const std::string &arg2 = "", const std::string &arg3 = "");

    virtual void InitErrorMessage() override;
};

#endif
