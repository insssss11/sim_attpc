#ifndef BranchAdderBase_h
#define BranchAdderBase_h 1

#include "TFile.h"
#include "TTreeReader.h"
#include "TTreeReaderValue.h"

#include <vector>
#include <string>
#include <iomanip>
#include <exception>
#include <memory>

using ReaderPtrs = std::vector<std::unique_ptr<TTreeReader> >;

template <typename dType>
using ReaderValuePtrs = std::vector<std::unique_ptr<TTreeReaderValue<dType> > >;

template <typename dType>
// Add up data of branches of multiple TTree instances and append to a given TTree.
class BranchAdderBase
{
    public:
    // All branch names must be identical.
    BranchAdderBase(const std::string &branchName);

    virtual ~BranchAdderBase();

    void AppendColumn(TTree *treeToAppend);
    bool CheckConsistency();

    void AddTreeFromFile(const std::string &treeName, const std::string &fileName);
    void AddTree(TTree *tree);

    void Reset();
    Long64_t GetEntries() const;
    std::string GetDataType() const;
    int GetNreaders() const;

    private:
    
    template <typename vdType>
    void AppendBranch(std::vector<vdType> *data, TTree *treeToAppend);
    void AppendBranch(dType *data, TTree *treeToAppend);

    void FillBranch(dType *data, TTree *treeToAppend);

    template <typename vdType>
    void FillSummedData(std::vector<vdType> *data);
    void FillSummedData(dType *data);

    void AddRootFile(TFile *rootFile);
    void AddReaderAndValue(TTree *tree);

    void CheckBranch(TTree *tree) const;
    void CheckReader(TTreeReader *readerPtr) const;
    void CheckReaderValue(TTreeReaderValue<dType> *readerValuePtr) const;

    void InitBranchAtts(TTree *tree);
    std::string GetDataType(TTree *tree) const;

    void ResetReaders();
    bool NextReaders();

    void CloseRootFiles();
    private:
    class Exception;
    const std::string branchName;

    dType data;
    int nReaders;
    ReaderPtrs readerPtrs;
    ReaderValuePtrs<dType> readerValuePtrs;
    std::vector<TFile *> openedRootFiles;

    // attributes going to be initialized when first tree is added.
    std::string dataType;
    Long64_t entries;
    Long64_t curEntry;
};

template <typename dType>
BranchAdderBase<dType>::BranchAdderBase(const std::string &_branchName)
    : branchName(_branchName), nReaders(0), entries(0), curEntry(0)
{
}

template <typename dType>
BranchAdderBase<dType>::~BranchAdderBase()
{
    CloseRootFiles();
}

template <typename dType>
void BranchAdderBase<dType>::CloseRootFiles()
{
    for(auto rootFile : openedRootFiles)
        rootFile->Close();
}

#include <typeinfo>
template <typename dType>
void BranchAdderBase<dType>::AppendColumn(TTree *treeToAppend)
{
    if(nReaders == 0)
        throw Exception("No tree to be added up. The number of trees must not be zero.");
    try {
        /*
        std::cout << typeid(data).name() << std::endl;
        std::cout << typeid(&data).name() << std::endl;
        */
        AppendBranch(static_cast<dType*>(&data), treeToAppend);
        FillBranch(static_cast<dType*>(&data), treeToAppend);
    }
    catch(std::exception const &e)
    {
        std::cerr << "Error occured in BranchAdderBase<dType>::AppendColumn(TTree*)" << std::endl;
        std::cerr << e.what() << std::endl;
    }
}

template <typename dType>
template <typename vdType>
void BranchAdderBase<dType>::AppendBranch(std::vector<vdType> *vData, TTree *treeToAppend)
{
    std::cout << "For vector-type " << std::endl;
    treeToAppend->Branch(branchName.c_str(), &vData);
}

template <typename dType>
void BranchAdderBase<dType>::AppendBranch(dType *data, TTree *treeToAppend)
{
    std::cout << "For Normal type " << std::endl;
    treeToAppend->Branch(branchName.c_str(), data);
}


template <typename dType>
void BranchAdderBase<dType>::FillBranch(dType *vData, TTree *treeToAppend)
{
    std::cout << "For vector-type " << std::endl;
    ResetReaders();
    try {
        while(NextReaders())
        {
            FillSummedData(vData);
            treeToAppend->Fill();
            ++curEntry;
        }
    }
    catch(std::exception const &e)
    {
        std::cerr << "Error occured in FillBranch(std::vector<vdType> *, TTree *)" << std::endl;
        std::cerr << e.what() << std::endl;
        throw Exception("Failed sum over entries at the event number " + std::to_string(curEntry));
    }
}

template <typename dType>
template <typename vdType>
void BranchAdderBase<dType>::FillSummedData(std::vector<vdType> *vData)
{
    std::vector<vdType> _vData;
    size_t vecSize = 0;
    for(auto &readerValuePtr : readerValuePtrs)
    {
        _vData = **readerValuePtr;
        if(vecSize == 0)
        {
            vecSize = _vData.size();
            vData->assign(vecSize, 0);
        }
        else if(vecSize != _vData.size())
            throw Exception("The size of vector at the entry number "  + std::to_string(curEntry) + "of the tree \"" +
            readerValuePtr->GetTree()->GetName() + "\" is not consistent.");
        for(size_t i = 0;i < vecSize;++i)
            vData->at(i) += _vData.at(i);
    }
}

template <typename dType>
void BranchAdderBase<dType>::FillSummedData(dType *data)
{
    *data = 0;
    for(auto &readerValuePtr : readerValuePtrs)
    {
        *data += **readerValuePtr;
    }
}

template <typename dType>
void BranchAdderBase<dType>::ResetReaders()
{
    curEntry = 0;
    for(auto &readerPtr : readerPtrs)
        readerPtr->Restart();
}

template <typename dType>
bool BranchAdderBase<dType>::NextReaders()
{
    for(auto &readerPtr : readerPtrs)
        if(!readerPtr->Next())
            return false;
    return true;
}

template <typename dType>
void BranchAdderBase<dType>::AddTreeFromFile(const std::string &treeName, const std::string &fileName)
{
    try {
        TFile *rootFile = TFile::Open(fileName.c_str(), "READ");
        AddRootFile(rootFile);
        TTree *tree = static_cast<TTree*>(rootFile->Get(treeName.c_str()));
        if(nReaders == 0)
            InitBranchAtts(tree);
        AddReaderAndValue(tree);
    }
    catch(std::exception const &e)
    {
        std::cerr << "Error occured in BranchAdderBase<dType>::AddTreeFromFile(const std::string &, const std::string &)" << std::endl;
        std::cerr << e.what() << std::endl;
        throw Exception("Failed to add reader and readerValue from a tree in a given root file.");
    }
}

template <typename dType>
void BranchAdderBase<dType>::AddRootFile(TFile *rootFile)
{
    if(!rootFile->IsOpen())
        throw Exception(std::string("Faied to open root file \"") + rootFile->GetName() + "\".");
    openedRootFiles.push_back(rootFile);
}

template <typename dType>
void BranchAdderBase<dType>::AddTree(TTree *tree)
{
    try {
        if(nReaders == 0)
            InitBranchAtts(tree);
        AddReaderAndValue(tree);
    }
    catch(std::exception const &e)
    {
        std::cerr << "Error occured in BranchAdderBase<dType>::AddTree(TTree *tree)" << std::endl;
        std::cerr << e.what() << std::endl;
        throw Exception("Failed to add reader and readerValue from a tree.");
    }
}

template <typename dType>
void BranchAdderBase<dType>::InitBranchAtts(TTree *tree)
{
    try {
        dataType = GetDataType(tree);
        entries = tree->GetEntries();
    }
    catch(std::exception const &e)
    {
        std::cerr << "Error occured in BranchAdderBase<dType>::InitBranchAtts(TTreeReader *, TTreeReaderValue<dType> *)" << std::endl;
        throw e;
    }
}

template <typename dType>
void BranchAdderBase<dType>::AddReaderAndValue(TTree *tree)
{
    std::unique_ptr<TTreeReader> readerPtr(new TTreeReader(tree));
    std::unique_ptr<TTreeReaderValue<dType> > readerValuePtr(new TTreeReaderValue<dType>(*readerPtr.get(), branchName.c_str()));
    try {
        // TTreeReader::GetEntryStatus() returns valid output only when Next() has been called more than one time.
        readerPtr->Next();
        CheckReader(readerPtr.get());
        CheckReaderValue(readerValuePtr.get());
    }
    catch(std::exception const &e)
    {
        std::cerr << "Error occured in BranchAdderBase<dType>::AddReaderAndValue(TTree *)" << std::endl;
        std::cerr << e.what() << std::endl;
    }
    readerPtrs.push_back(std::move(readerPtr));
    readerValuePtrs.push_back(std::move(readerValuePtr));
    ++nReaders;
}

template <typename dType>
std::string BranchAdderBase<dType>::GetDataType(TTree *tree) const
{
    return std::string(tree->GetBranch(branchName.c_str())->GetLeaf(branchName.c_str())->GetTypeName());
}

template <typename dType>
void BranchAdderBase<dType>::CheckBranch(TTree *tree) const
{
    if(entries != tree->GetEntries())
        throw Exception(std::string("The number of the tree \"") +
        tree->GetName() + "\" is not consistent with those of others");
    else if(dataType != GetDataType(tree))
        throw Exception("The data type of the branch \"" + branchName + "\" int the tree \"" +
        tree->GetName() + "\" is not consistent with those of others");
}

template <typename dType>
void BranchAdderBase<dType>::CheckReader(TTreeReader *readerPtr) const
{
    if(readerPtr->GetEntryStatus() != TTreeReader::kEntryValid)
        throw Exception(std::string("Reader entry status for the tree \"") +
        readerPtr->GetTree()->GetName() + "\" : " + std::to_string(static_cast<int>(readerPtr->GetEntryStatus())));
}

template <typename dType>
void BranchAdderBase<dType>::CheckReaderValue(TTreeReaderValue<dType> *readerValuePtr) const
{
    if(readerValuePtr->GetSetupStatus() < 0)
    {
        throw Exception(std::string("Setting up reader for \"") + readerValuePtr->GetBranchName() +
            "\" status : " + std::to_string(static_cast<int>(readerValuePtr->GetSetupStatus())));
    }
}

template <typename dType>
void BranchAdderBase<dType>::Reset()
{
    CloseRootFiles();
    readerPtrs.clear();
    readerValuePtrs.clear();
    nReaders = 0;
    entries = 0;
    curEntry = 0;
    dataType = "";
}

template <typename dType>
Long64_t BranchAdderBase<dType>::GetEntries() const
{
    return entries;
}

template <typename dType>
std::string BranchAdderBase<dType>::GetDataType() const
{
    return dataType;
}

template <typename dType>
int BranchAdderBase<dType>::GetNreaders() const
{
    return nReaders;
}

template <typename dType>
class BranchAdderBase<dType>::Exception : public std::exception {
    public:
    Exception(const std::string &_message) : std::exception(), message("what() : " + _message) {}
    const char *what() const noexcept override
    {
        return message.c_str();
    }
    private:
    const std::string message;
};

#endif

