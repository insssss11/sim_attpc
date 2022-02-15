/// \file EventAction.hh
/// \brief Definition of the EventAction class

#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "G4GenericMessenger.hh"

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
    void PrintBeginOfEvent();
    // for UI command
    void DefineCommands();
    
    private:
    G4GenericMessenger *fMessenger;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
