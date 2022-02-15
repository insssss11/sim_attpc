/// \file EventAction.cc
/// \brief Implementation of the EventAction class

#include "EventAction.hh"
#include "config/ParamContainerTable.hh"
#include "gas_chamber/GasChamberHit.hh"
#include "gas_chamber/GasChamberDigitizer.hh"
#include "AnalysisManager.hh"

#include "G4UnitsTable.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4SDManager.hh"
#include "G4DigiManager.hh"
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
    // initialize vector container for each tuple
    InitNtuplesVectorGasChamber();
    auto container = ParamContainerTable::GetContainer("gas_chamber");
    auto dmManager = G4DigiManager::GetDMpointer();
    // Digitizer
    GasChamberDigitizer *gasChamberDigitizer = new GasChamberDigitizer(
        "GasChamberDigitizer", container->GetParamD("chamberX"), container->GetParamD("chamberY"),
        container->GetParamI("nPadX"), container->GetParamI("nPadY"),
        43.3*eV,
        G4ThreeVector(container->GetParamD("posX"), container->GetParamD("posY"), container->GetParamD("posZ")));
    gasChamberDigitizer->SetPadMargin(container->GetParamD("margin"));
    gasChamberDigitizer->SetChargeMultiplication(container->GetParamD("multiplication"));
    dmManager->AddNewModule(gasChamberDigitizer);
    G4cout << "Fuck" << G4endl;

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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

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

#include "tuple_vector/TupleVectorManager.hh"

void EventAction::InitNtuplesVectorGasChamber()
{
    auto tupleVectorManager = TupleVectorManager::Instance();
    tupleVectorManager->AddTupleVectorContainer("tree_gc2")
        ->AddVectorsD("x", "y", "z", "px", "py", "pz", "eDep", "t", "q", "stepLen");
    tupleVectorManager->AddTupleVectorContainer("tree_gc3")
        ->AddVectorsD("qdc");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::FillNtupleGasChamber()
{
    auto hitCol = static_cast<const GasChamberHitsCollection *>(GetHC(G4RunManager::GetRunManager()->GetCurrentEvent(), fGasChamberHcId));
    auto analysisManager = G4AnalysisManager::Instance();
    auto tupleVectorManager = TupleVectorManager::Instance();

    // tuple saved by event
    analysisManager->FillNtupleIColumn(0, 0, hitCol->GetSize());
    analysisManager->AddNtupleRow(0);

    // tuple saved by track
    auto tupleVector2 = tupleVectorManager->GetTupleVectorContainer("tree_gc2");
    for(size_t i = 0;i < hitCol->GetSize();++i)
    {
        auto hit = static_cast<GasChamberHit *>(hitCol->GetHit(i));
        analysisManager->FillNtupleIColumn(1, 0, G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID());
        analysisManager->FillNtupleIColumn(1, 1, hit->GetTrackId());
        analysisManager->FillNtupleIColumn(1, 2, hit->GetNbOfStepPoints());
        analysisManager->FillNtupleIColumn(1, 3, hit->GetAtomicNumber());
        analysisManager->FillNtupleDColumn(1, 4, hit->GetMass());
        analysisManager->FillNtupleDColumn(1, 5, hit->GetTrackLength());
        analysisManager->FillNtupleDColumn(1, 6, hit->GetEdepSum());
        // analysisManager->FillNtupleSColumn(1, 7, hit->GetPartName());

        // vector part
        tupleVector2->FillVectorD("x", hit->GetPosX());
        tupleVector2->FillVectorD("y", hit->GetPosY());
        tupleVector2->FillVectorD("z", hit->GetPosZ());
        tupleVector2->FillVectorD("px", hit->GetMomX());
        tupleVector2->FillVectorD("py", hit->GetMomY());
        tupleVector2->FillVectorD("pz", hit->GetMomZ());
        tupleVector2->FillVectorD("eDep", hit->GetEdep());
        // tupleVector2->FillVectorD("t", hit->GetTime());
        // tupleVector2->FillVectorD("q", hit->GetCharge());
        tupleVector2->FillVectorD("stepLen", hit->GetStepLen());
        analysisManager->AddNtupleRow(1);
        tupleVector2->ClearVectors();
    }
    auto dmManager = G4DigiManager::GetDMpointer();
    GasChamberDigitizer *gasChamberDigitizer =
        static_cast<GasChamberDigitizer*>(dmManager->FindDigitizerModule("GasChamberDigitizer"));
    // digitization
    auto tupleVector3 = tupleVectorManager->GetTupleVectorContainer("tree_gc3");
    gasChamberDigitizer->FillPadsInfo(hitCol);
    tupleVector3->FillVectorD("qdc", gasChamberDigitizer->GetChargeOnPads());
    analysisManager->AddNtupleRow(2);
    tupleVector3->ClearVectors();
    gasChamberDigitizer->ClearPads();
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