#ifndef BranchAppender_h
#define BranchAppender_h 1

#include "TTree.h"
#include <vector>

// Append branch to a given tree.
template <typename dType>
class BranchAppender
{
    public:
    BranchAppender(const std::string &branchName);
    virtual ~BranchAppender(){}

    void AppendBranch(dType *&data, TTree *treeToAppend);

    private:
    const std::string branchName;
};

template <typename dType>
BranchAppender<dType>::BranchAppender(const std::string &_branchName)
    : branchName(_branchName)
{
}

template <typename dType>
void BranchAppender<dType>::AppendBranch(dType *&data, TTree *treeToAppend)
{
    treeToAppend->Branch(branchName.c_str(), data);
}

// Specialization for a vector-type template parameter.
template <typename vdType>
class BranchAppender<std::vector<vdType> >
{
    public:
    BranchAppender(const std::string &branchName);
    virtual ~BranchAppender(){}

    void AppendBranch(std::vector<vdType> *&data, TTree *treeToAppend);

    private:
    const std::string branchName;
};

template <typename vdType>
BranchAppender<std::vector<vdType> >::BranchAppender(const std::string &_branchName)
    : branchName(_branchName)
{
}

template <typename vdType>
void BranchAppender<std::vector<vdType> >::AppendBranch(std::vector<vdType> *&data, TTree *treeToAppend)
{
    treeToAppend->Branch(branchName.c_str(), &data);
}
#endif

