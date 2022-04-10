/// \file TreeMerger.hh
/// \brief Definition of the TreeMerger class

#ifndef TreeMerger_h
#define TreeMerger_h 1

#include "globals.hh"

#include "TFile.h"
#include "TTree.h"

#include <string>
#include <memory>
#include <utility>

class TreeMerger
{
    public:
    TreeMerger(G4int nThreads, const std::string fileName);
    
    void MergeRootFiles(G4bool deleteMerged = true);

    TreeMerger &AddTreeNameTitle(const std::string &treeName, const std::string &treeTitle = "");

    private:
    void OpenThreadFiles();
    void CloseThreadFiles();
    void DeleteThreadFiles();

    G4bool CheckOpened(const TFile *rootFile) const;

    void MergeTrees();
    TList *ExtractThreadTrees(const std::string &treeName);
    TTree *GetTreeFromFile(const std::string &treeName, TFile *rootFile);

    private:
    const G4int nThreads;
    const std::string fileName;
    // The trees with given name must exists in all thread root files.
    std::vector<std::pair<std::string, std::string> > treeNameTitles;
    std::vector<std::unique_ptr<TFile> > openedThreadFiles;
};

#endif
