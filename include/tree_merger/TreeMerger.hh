/// \file TreeMerger.hh
/// \brief Definition of the TreeMerger class

#ifndef TreeMerger_h
#define TreeMerger_h 1

#include "globals.hh"

#include "TFile.h"
#include "TTree.h"
#include "TList.h"

#include <string>
#include <memory>
#include <utility>

class TreeMerger
{
    public:
    TreeMerger();
    TreeMerger &AddTreeNameTitle(const std::string &treeName, const std::string &treeTitle = "");
    void ClearTreeNameTitles();
    
    void MergeRootFiles(const std::string &fileName, const std::vector<std::string> subFileNames);

    private:

    TTree *MergeTrees(const std::string &treeName, const std::string &treeTitle);
    void ClearRootFiles(); // flush all in-memory objects loaded from root files.

    void OpenSubFiles();
    void CloseSubFiles();
    void DeleteSubFiles();

    G4bool CheckOpened(const TFile *rootFile) const;
    TTree *GetTreeFromFile(const std::string &treeName, TFile *rootFile);

    private:
    std::string fileName;
    std::vector<std::string> subFileNames;
    
    // The trees with given name must exists in all thread root files.
    std::unique_ptr<TList> treeLists;
    std::unique_ptr<TFile> masterFile;
    
    std::vector<std::pair<std::string, std::string> > treeNameTitles;
    std::vector<std::unique_ptr<TFile> > openedSubFiles;
};

#endif
