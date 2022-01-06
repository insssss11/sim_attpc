/// \file EventAction.hh
/// \brief Definition of the EventAction class

#ifndef EventAction_h
#define EventAction_h 1


#include "G4UserEventAction.hh"
#include "G4GenericMessenger.hh"
#include "globals.hh"

#include <vector>
#include <array>
/// Event action

class EventAction : public G4UserEventAction
{
public:
    EventAction();
    virtual ~EventAction();
    
    virtual void BeginOfEventAction(const G4Event*);
    virtual void EndOfEventAction(const G4Event*);
    
protected:
    G4int verboseLevel;
private:
    // initialize hit collection id
    void InitHcIds();
    void PrintBeginOfEvent();

    void FillNtupleGasChamber();
    void PrintGasChamberHits();
    
    void DefineCommands();
private:
    G4bool fHcIdsInitialized;
    // hit collections Ids
    G4int fGasChamberHcId;
    G4GenericMessenger *fMessenger;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
