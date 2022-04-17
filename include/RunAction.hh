/// \file RunAction.hh
/// \brief Definition of the RunAction class

#ifndef RunAction_h
#define RunAction_h 1

#include "tree_merger/TreeMerger.hh"
#include "tuple/TupleInitializer.hh"
#include "RunActionMessenger.hh"

#include "AnalysisManager.hh"
#include "G4UserRunAction.hh"
#include "G4GenericMessenger.hh"
#include "globals.hh"

#include <memory>

class RunActionMessenger;
class TupleInitializer;
class EventAction;
class G4Run;

/// Run action class
class RunAction : public G4UserRunAction
{
    public:
    RunAction(EventAction *eventAction);
    virtual ~RunAction();

    virtual void BeginOfRunAction(const G4Run *);
    virtual void   EndOfRunAction(const G4Run *);

    // for messenger and UI
    void SetFileName(const std::string &fileName);
    void MergeFiles(const std::string &fileName, const std::vector<std::string> &subFiles);
    void Activate(G4bool activate = true);

    private:
    void MergeThreadTrees();

    private:
    G4bool fAnaActivated;
    G4String fFileName;
    EventAction *fEventAction;
    G4AnalysisManager *fAnalysisManager;
    std::unique_ptr<RunActionMessenger> messenger;
    std::unique_ptr<TreeMerger> merger;
    std::unique_ptr<TupleInitializer> tupleInitializer;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
