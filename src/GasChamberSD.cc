/// \file GasChamberSD.cc
/// \brief Implementation of the GasChamberSD class

#include "GasChamberSD.hh"
#include "GasChamberHit.hh"

#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4Step.hh"

#include "G4SDManager.hh"
#include "G4RunManager.hh"

#include "G4ios.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GasChamberSD::GasChamberSD(G4String name, G4int verbose)
    : G4VSensitiveDetector(name),
    fHitsCollection(nullptr), fMessenger(nullptr), fHCID(-1), fEventId(-1), fTrackId(-1),
    fSizeOfHCE(0), fNbOfStepPoints(0)
{
    verboseLevel = verbose;
    collectionName.insert("GasChamberHColl");
    DefineCommands();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GasChamberSD::~GasChamberSD()
{
    delete fMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberSD::Initialize(G4HCofThisEvent *hce)
{
    fHitsCollection = new GasChamberHitsCollection(SensitiveDetectorName, collectionName[0]);
    if(fHCID < 0)
    {
        fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection);
    }
    hce->AddHitsCollection(fHCID, fHitsCollection);

    fNbOfStepPoints = 0;
    fSizeOfHCE = 0;
    fEventId = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
    fTrackId = -1;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberSD::EndOfEvent(G4HCofThisEvent *HCE)
{
    // fill the # of the step points of the last track if more than one.
    if(fNbOfStepPoints > 0)
        (*fHitsCollection)[fSizeOfHCE - 1]->SetNbStepPoints(fNbOfStepPoints);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool GasChamberSD::ProcessHits(G4Step *step, G4TouchableHistory *)
{
    static G4int cnt = kNbOfSkipPrint;

    auto preStepPoint = step->GetPreStepPoint();
    auto postStepPoint = step->GetPreStepPoint();
    const auto track = step->GetTrack();
    const auto pDynamic = step->GetTrack()->GetDynamicParticle();
    const auto pDefinition = pDynamic->GetParticleDefinition();
    // if new track
    if(fTrackId != track->GetTrackID())
    {
        if(fSizeOfHCE > 0)
            (*fHitsCollection)[fSizeOfHCE - 1]->SetNbStepPoints(fNbOfStepPoints);
        ++fSizeOfHCE;
        fNbOfStepPoints = 0;
        fTrackId = track->GetTrackID();
        fHitsCollection->insert(new GasChamberHit(pDynamic, fEventId, fTrackId));
    }
    auto hit = (*fHitsCollection)[fSizeOfHCE - 1];
    hit->AppendEdep(step->GetTotalEnergyDeposit());
    hit->AppendPosition(track->GetPosition());
    hit->AppendMomentum(track->GetMomentum());
    hit->AddEdepSum(step->GetTotalEnergyDeposit());
    hit->AddTrackLength(step->GetStepLength());
    ++fNbOfStepPoints;
    return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberSD::DefineCommands()
{
    fMessenger = new G4GenericMessenger(this, "/attpc/gasChamber/", "Gas Chamger SD control");
    auto fVerboseCmd = fMessenger->DeclareProperty("verbose", verboseLevel, "Set verbosity");
}