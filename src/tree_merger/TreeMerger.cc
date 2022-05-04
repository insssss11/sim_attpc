/// \file TreeMerger.cc
/// \brief Implementation of the TreeMerger class

#include <stdio.h>

#include "tree_merger/TreeMerger.hh"
#include "tree_merger/TreeMergerException.hh"

using namespace std;
using namespace TreeMergerErrorNum;

TreeMerger::TreeMerger()
    :treeLists(new TList())
{
}

TreeMerger &TreeMerger::AddTreeNameTitle(const string &treeName, const string &treeTitle)
{
    treeNameTitles.emplace_back(treeName, treeTitle);
    return *this;
}

void TreeMerger::ClearTreeNameTitles()
{
    treeNameTitles.clear();
}

void TreeMerger::MergeRootFiles(const std::string &fileName, const std::vector<std::string> subFileNames)
{
    if(treeNameTitles.empty())
    {
        G4cerr << "No tree names have been added. Aborting merge." << G4endl;
        return;
    }
    try {
        if(subFileNames.size() < 2)
            throw invalid_argument("The number of files to be merged must be larger than two.");
        this->subFileNames = subFileNames;
        masterFile = make_unique<TFile>(fileName.c_str(), "RECREATE");
        OpenSubFiles();
        for(const auto &treeNameTitle : treeNameTitles)
        {
            TTree *mergedTree = MergeTrees(treeNameTitle.first, treeNameTitle.second); 
            mergedTree->Write();
            G4cout << "Successfully merged and saved " + treeNameTitle.first + "." << G4endl;
            ClearRootFiles();
        }
        CloseSubFiles();
        masterFile->Close();
        G4cout << "Successfully merged and saved all trees into " << fileName + "." << G4endl;
        DeleteSubFiles();
        G4cout << "Successfully deleted all thread root files."<< G4endl;
    } 
    catch(Exception const &e)
    {
        e.WarnGeantKernel(JustWarning);
        CloseSubFiles();
    }
}

void TreeMerger::OpenSubFiles()
{
    CloseSubFiles();
    for(const auto &subFileName : subFileNames)
    {
        auto rootFile = TFile::Open(subFileName.c_str(), "READ");
        if(!CheckOpened(rootFile))
            throw TreeMergerException("OpenSubFiles()", THREAD_FILE_OPEN_FAILURE, subFileName);
        openedSubFiles.emplace_back(rootFile);
    }
}

G4bool TreeMerger::CheckOpened(const TFile *rootFile) const
{
    if(rootFile == nullptr)
        return false;
    else
        return rootFile->IsOpen();
}

void TreeMerger::CloseSubFiles()
{
    for(auto &files : openedSubFiles)
        files->Close();
    openedSubFiles.clear();
}

TTree *TreeMerger::MergeTrees(const string &treeName, const string &treeTitle)
{
    treeLists->Clear();
    for(auto &threadFile : openedSubFiles)
    {
        TTree *tree = GetTreeFromFile(treeName, threadFile.get());
        treeLists->Add(tree);
    }
    masterFile->cd();
    TTree *treeMerged = TTree::MergeTrees(treeLists.get());
    if(treeMerged == nullptr)
        throw TreeMergerException("TreeMerger::MergeTrees", MERGED_TREE_EMPTY, treeName);
    treeMerged->SetNameTitle(treeName.c_str(), treeTitle.c_str());
    return treeMerged;  
}

TTree *TreeMerger::GetTreeFromFile(const string &treeName, TFile *rootFile)
{
    auto tree = static_cast<TTree*>(rootFile->Get(treeName.c_str()));
    if(tree == nullptr)
        throw TreeMergerException("GetTreeFromFile(const string &, TFile *)", TREE_NOT_FOUND, treeName);
    return tree;
}

void TreeMerger::ClearRootFiles()
{
    masterFile->Clear();
    for(auto &threadFile : openedSubFiles)
        threadFile->Clear();
}

void TreeMerger::DeleteSubFiles()
{
    for(const auto &subFileName : subFileNames)
    {
        try{
            if(remove(subFileName.c_str()) != 0)
                throw TreeMergerException("DeleteSubFiles()", THREAD_FILE_REMOVE_FAILURE, subFileName);
            G4cout << "Merged and deleted " + subFileName << "." << G4endl;
        }
        catch(Exception const &e)
        {
            e.WarnGeantKernel(JustWarning);
        }
    }
}