/// \file RunAction.hh
/// \brief Definition of the RunAction class

#ifndef RunAction_h
#define RunAction_h 1

#include "AnalysisManager.hh"
#include "G4UserRunAction.hh"
#include "G4GenericMessenger.hh"
#include "globals.hh"

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

    private:
    // create tuples in AnalysisManager for each detector SD
    void CreateTuplesGasChamber();

    // for messenger and UI
    void DefineCommands();
    private:
    G4bool fAnaActivated;
    G4String fFileName;
    EventAction *fEventAction;
    G4AnalysisManager *fAnalysisManager;
    G4GenericMessenger *fMessenger;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
