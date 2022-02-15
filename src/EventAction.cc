/// \file EventAction.cc
/// \brief Implementation of the EventAction class

#include "EventAction.hh"
#include "AnalysisManager.hh"

#include "G4UnitsTable.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"
#include "G4ios.hh"

// Utility function which finds a hit collection with the given Id
// and print warnings if not found 
G4VHitsCollection *GetHC(const G4Event *event, G4int collId)
{
    auto hce = event->GetHCofThisEvent();
    if(!hce)
    {
        G4ExceptionDescription msg;
        msg << "No hits collection of this event found." << G4endl;
        G4Exception("EventAction::EndOfEventAction()",
            "Code001", JustWarning, msg);
        return nullptr;
    }

    auto hc = hce->GetHC(collId);
    if(!hc)
    {
        G4ExceptionDescription msg;
        msg << "Hits collection " << collId << " of this event not found." << G4endl;
        G4Exception("EventAction::EndOfEventAction()",
            "Code001", JustWarning, msg);
    }
    return hc;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction()
    : G4UserEventAction(),
    verboseLevel(0)
{
    // set printing per each event
    G4RunManager::GetRunManager()->SetPrintProgress(1);
    DefineCommands();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::~EventAction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event *)
{
    PrintBeginOfEvent();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event *)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::PrintBeginOfEvent()
{
    // printing out information of primary particle
    auto primary = G4RunManager::GetRunManager()->GetCurrentEvent()->GetPrimaryVertex()->GetPrimary();
    if(verboseLevel > 0)
    {
        int prec = G4cout.precision(4);
        G4cout << "--------------------------------------------------------------------------------------------------------------------------------" << G4endl;
        G4cout << std::setw(40) << std::left << "Primary particle " <<  " : " << std::setw(10) << std::right;
        // put "Generic Ion" in front of a partice name if it is a generic ion.
        if(primary->GetParticleDefinition()->IsGeneralIon())
            G4cout << " Generic Ion ";
        G4cout << primary->GetParticleDefinition()->GetParticleName() << " " << (G4int)primary->GetCharge() << "+" << G4endl;
        G4cout << std::setw(40) << std::left << "Atomic Mass" << " : " << std::setw(10) << std::right
            << primary->GetParticleDefinition()->GetPDGMass()/(931.5*MeV) << " u" << G4endl;
        G4cout << std::setw(40) << std::left << "Kinetic Energy" << " : " << std::setw(10) << std::right
            << G4BestUnit(primary->GetKineticEnergy(), "Energy") << G4endl;
        G4cout.precision(prec);
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::DefineCommands()
{
    fMessenger = new G4GenericMessenger(this, "/attpc/event/", "Overall run control");
    // auto fVerboseCmd = fMessenger->DeclareProperty("verbose", verboseLevel, "Set verbosity");
    fMessenger->DeclareProperty("verbose", verboseLevel, "Set verbosity");
}