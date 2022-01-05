/// \file RunAction.hh
/// \brief Definition of the RunAction class

#ifndef RunAction_h
#define RunAction_h 1

#include "G4GenericAnalysisManager.hh"
#include "G4UserRunAction.hh"
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
    EventAction *fEventAction;
    G4GenericAnalysisManager *fAnalysisManager;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
