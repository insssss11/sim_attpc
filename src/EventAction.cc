/// \file EventAction.cc
/// \brief Implementation of the EventAction class

#include "EventAction.hh"
#include "GasChamberHit.hh"
#include "AnalysisManager.hh"

#include "G4UnitsTable.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4SDManager.hh"
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
    verboseLevel(0), fHcIdsInitialized(false), fGasChamberHcId(-1)
{
    // set printing per each event
    G4RunManager::GetRunManager()->SetPrintProgress(1);
    DefineCommands();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::~EventAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event *)
{
    if(!fHcIdsInitialized)
    {
        fHcIdsInitialized = true;
        InitHcIds();
    }
    PrintBeginOfEvent();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::EndOfEventAction(const G4Event *)
{
    FillNtupleGasChamber();
    PrintGasChamberHits();
}

void EventAction::InitHcIds()
{
    auto sdManager = G4SDManager::GetSDMpointer();
    fGasChamberHcId = sdManager->GetCollectionID("gasChamber/GasChamberHColl");
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

void EventAction::FillNtupleGasChamber()
{
    auto hitCol = GetHC(G4RunManager::GetRunManager()->GetCurrentEvent(), fGasChamberHcId);
    auto analysisManager = G4AnalysisManager::Instance();

    // tuple saved by event
    analysisManager->FillNtupleIColumn(0, 0, hitCol->GetSize());
    analysisManager->AddNtupleRow(0);

    // tuple saved by track
    for(size_t i = 0;i < hitCol->GetSize();++i)
    {
        auto hit = static_cast<GasChamberHit *>(hitCol->GetHit(i));
        analysisManager->FillNtupleIColumn(1, 0, G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID());
        analysisManager->FillNtupleIColumn(1, 1, hit->GetNbOfStepPoints());
        analysisManager->FillNtupleIColumn(1, 2, hit->GetAtomicNumber());
        analysisManager->FillNtupleDColumn(1, 3, hit->GetMass());
        analysisManager->FillNtupleDColumn(1, 4, hit->GetTrackLength());
        analysisManager->FillNtupleDColumn(1, 5, hit->GetEdepSum());
        analysisManager->FillNtupleSColumn(1, 6, hit->GetPartName());
        analysisManager->AddNtupleRow(1);
    }

    // tuple saved by step
    for(size_t i = 0;i < hitCol->GetSize();++i)
    {
        auto hit = static_cast<GasChamberHit *>(hitCol->GetHit(i));
        for(int j = 0;j < hit->GetNbOfStepPoints();++j)
        {
            analysisManager->FillNtupleDColumn(2, 0, hit->GetPosX().at(j));
            analysisManager->FillNtupleDColumn(2, 1, hit->GetPosY().at(j));
            analysisManager->FillNtupleDColumn(2, 2, hit->GetPosZ().at(j));
            analysisManager->FillNtupleDColumn(2, 3, hit->GetMomX().at(j));
            analysisManager->FillNtupleDColumn(2, 4, hit->GetMomY().at(j));
            analysisManager->FillNtupleDColumn(2, 5, hit->GetMomZ().at(j));
            analysisManager->FillNtupleDColumn(2, 6, hit->GetEdep().at(j));
            analysisManager->FillNtupleDColumn(2, 7, hit->GetTime().at(j));
            analysisManager->FillNtupleDColumn(2, 8, hit->GetCharge().at(j));
            analysisManager->AddNtupleRow(2);
        }
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::PrintGasChamberHits()
{
    if(verboseLevel > 0)
    {
        auto hitCol = GetHC(G4RunManager::GetRunManager()->GetCurrentEvent(), fGasChamberHcId);
        int prec = G4cout.precision(4);
        G4cout << "--------------------------------------------------------------------------------------------------------------------------------" << G4endl;
        G4cout << std::setw(40) << std::left << "The # of tracks in this event" << " : " << std::setw(10) << std::right << hitCol->GetSize() << G4endl;
        G4cout << "--------------------------------------------------------------------------------------------------------------------------------" << G4endl;
        if(verboseLevel > 1)
        {
            for(size_t i = 0;i < hitCol->GetSize();++i)
            {
                auto hit = static_cast<GasChamberHit *>(hitCol->GetHit(i));
                G4cout << std::setw(3) << i + 1 << std::left <<  std::setw(37) << "-th track info" << " : "
                    << std::setw(10) << std::right << hit->GetPartName()
                    << " with " << std::setw(10) << hit->GetNbOfStepPoints() << " steps." << G4endl;
                if(verboseLevel > 2)
                {
                    G4cout << std::setw(13) << "PosX" << std::setw(13) << "PosY" << std::setw(13) << "PosZ"
                        << std::setw(14) << "DirX" << std::setw(10) << "DirY" << std::setw(10) << "DirZ"
                        << std::setw(14) << "Edep" << std::setw(14) << "kinE" << std::setw(14) << "Charge" << G4endl;
                    hit->Print();
                }
            }
            G4cout << "--------------------------------------------------------------------------------------------------------------------------------" << G4endl;

        }
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