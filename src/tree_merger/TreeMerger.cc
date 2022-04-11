/// \file TreeMerger.cc
/// \brief Implementation of the TreeMerger class

#include <stdio.h>

#include "tree_merger/TreeMerger.hh"
#include "tree_merger/TreeMergerException.hh"

using namespace std;
using namespace TreeMergerErrorNum;

TreeMerger::TreeMerger(G4int _nThreads, const string _fileName)
    :nThreads(_nThreads), fileName(_fileName), treeLists(new TList())
{
}

TreeMerger &TreeMerger::AddTreeNameTitle(const string &treeName, const string &treeTitle)
{
    treeNameTitles.emplace_back(treeName, treeTitle);
    return *this;
}

void TreeMerger::MergeRootFiles(G4bool deleteMerged)
{
    if(treeNameTitles.empty())
    {
        G4cerr << "No tree names have been added. Aborting merge." << G4endl;
        return;
    }
    try {
        masterFile = make_unique<TFile>(fileName.c_str(), "RECREATE");
        OpenThreadFiles();
        for(const auto &treeNameTitle : treeNameTitles)
        {
            TTree *mergedTree = MergeTrees(treeNameTitle.first, treeNameTitle.second); 
            cout << "Fuck1" << endl;            
            mergedTree->Write();
            cout << "Fuck1" << endl;            
            G4cout << "Successfully merged and saved " + treeNameTitle.first + "." << G4endl;
            ClearRootFiles();
        }
        cout << "Fuck1" << endl;
        CloseThreadFiles();
        masterFile->Close();
        G4cout << "Successfully merged and saved all trees into " << fileName + "." << G4endl;
        if(deleteMerged)
            DeleteThreadFiles();
        G4cout << "Successfully deleted all thread root files."<< G4endl;
    } 
    catch(Exception const &e)
    {
        e.WarnGeantKernel(JustWarning);
        CloseThreadFiles();
    }
}

void TreeMerger::OpenThreadFiles()
{
    CloseThreadFiles();
    for(int i = 0;i < nThreads;++i)
    {
        auto threadRootFile = fileName.substr(0, fileName.find_last_of('.')) + "_t" + to_string(i) + ".root";
        auto rootFile = TFile::Open(threadRootFile.c_str(), "READ");
        if(!CheckOpened(rootFile))
            throw TreeMergerException("OpenThreadFiles()", THREAD_FILE_OPEN_FAILURE, threadRootFile);
        openedThreadFiles.emplace_back(rootFile);
    }
}

G4bool TreeMerger::CheckOpened(const TFile *rootFile) const
{
    if(rootFile == nullptr)
        return false;
    else
        return rootFile->IsOpen();
}

void TreeMerger::CloseThreadFiles()
{
    for(auto &files : openedThreadFiles)
        files->Close();
    openedThreadFiles.clear();
}

TTree *TreeMerger::MergeTrees(const string &treeName, const string &treeTitle)
{

    treeLists->Clear();
    for(auto &threadFile : openedThreadFiles)
    {
        TTree *tree = GetTreeFromFile(treeName, threadFile.get());
        treeLists->Add(tree);
    }
    masterFile->cd();
    TTree *treeMerged = TTree::MergeTrees(treeLists.get());
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
    for(auto &threadFile : openedThreadFiles)
        threadFile->Clear();
}

void TreeMerger::DeleteThreadFiles()
{
    for(int i = 0;i < nThreads;++i)
    {
        try{
            auto threadRootFile = fileName.substr(0, fileName.find_last_of('.')) + "_t" + to_string(i) + ".root";
            if(remove(threadRootFile.c_str()) != 0)
                throw TreeMergerException("DeleteThreadFiles()", THREAD_FILE_REMOVE_FAILURE, threadRootFile);
            G4cout << "Merged and deleted " + threadRootFile << "." << G4endl;
        }
        catch(Exception const &e)
        {
            e.WarnGeantKernel(JustWarning);
        }
    }
}

