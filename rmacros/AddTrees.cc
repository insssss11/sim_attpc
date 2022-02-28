
#include "BranchSummation/BranchSummation.hh"

#include <stdarg.h>

int AddTrees(const char *outFile, const char *treeName, const char *branchName, int nTrees, ...)
{
    BranchSummation<int > *adder = new BranchSummation<int>(branchName);
    va_list ap;
    va_start(ap, nTrees);
    for(int i = 0; i < nTrees; i++)
        adder->AddTreeFromFile(treeName, va_arg(ap, const char*));
    va_end(ap);
    auto rootOutFile = TFile::Open(outFile, "RECREATE");
    auto tree = new TTree(treeName, "");
    adder->AppendColumn(tree);
    rootOutFile->Write();
    rootOutFile->Close();
    return 0;
}