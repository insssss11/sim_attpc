/// \file TreeMergerException.cc
/// \brief Implementation of the TreeMergerException class

#include "tree_merger/TreeMergerException.hh"

using namespace TreeMergerErrorNum;
using namespace std;

TreeMergerException::TreeMergerException(
    const string &_originMethod, ErrorNum _errorNumber,  const string &arg1, const string &arg2, const string &arg3)
    : Exception("TreeMerger", "TreeMerger", _originMethod, _errorNumber, arg1, arg2, arg3)
{
    InitErrorMessage();
}

void TreeMergerException::InitErrorMessage()
{
    const string vecName = GetArgument(0);
    switch(GetErrorNum())
    {
        case THREAD_FILE_OPEN_FAILURE:
            message = "Failed to open a thread root file to merge : " + GetArgument(0);
            break;
        case THREAD_FILE_REMOVE_FAILURE:
            message = "Failed to remove merged thread root file : " + GetArgument(0);
            break;
        case TREE_NOT_FOUND:
            message = "Failed to find a tree from root file : " + GetArgument(0);
            break;
        case MERGED_TREE_EMPTY:
            message = "Merged tree is empty : " + GetArgument(0);
            break;
        case OK:
            break;
        default:
            message = "Unidentified error number : " + GetErrorCode();
            break;
    }
}