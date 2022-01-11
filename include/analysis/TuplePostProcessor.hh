/// \file TuplePostProcessor.hh
/// \brief Definition of the TuplePostProcessor class

#ifndef TuplePostProcessor_h
#define TuplePostProcessor_h 1

#include "TFile.h"
#include "TTree.h"
#include "AnalysisManager.hh"
/// This rearrange and rewrite ntuple to TTree with 2-dimensional array form.
class TuplePostProcessor
{
    public:
    TuplePostProcessor(const char *fileName);
    virtual ~TuplePostProcessor();

    // postprocess ntuples
    void PostProcessTuples();
    void Write();
    void Close();

    private:
    void PostProcessTuplesGasChamber();

    TFile *fRootFile;
    char fFileName[256];
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
