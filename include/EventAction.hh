/// \file EventAction.hh
/// \brief Definition of the EventAction class

#ifndef EventAction_h
#define EventAction_h 1

#include "tuple_vector/TupleVectorContainer.hh"

#include "G4UserEventAction.hh"
#include "G4GenericMessenger.hh"
#include "G4DigiManager.hh"

#include "globals.hh"

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

    // method handling tuples for each detector SD
    // Initiated in the constructor and filled, printed in the EndOfEventAction()

    // for gas chamber SD
    void InitNtuplesVectorGasChamber();
    void FillNtupleGasChamber();
    void PrintGasChamberHits();

    // method for another SD can be added in the same way

    // for UI command
    void DefineCommands();
    
    private:
    G4bool fHcIdsInitialized;
    // hit collections Ids
    G4int fGasChamberHcId;
    G4GenericMessenger *fMessenger;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
