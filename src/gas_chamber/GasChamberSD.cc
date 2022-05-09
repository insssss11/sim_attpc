/// \file GasChamberSD.cc
/// \brief Implementation of the GasChamberSD class
#include "tuple/TupleInitializerBase.hh"
#include "gas_chamber/GasChamberSD.hh"
#include "gas_chamber/GasChamberHit.hh"
#include "config/ParamContainerTable.hh"

#include "G4HCofThisEvent.hh"
#include "G4TouchableHistory.hh"
#include "G4Track.hh"
#include "G4Step.hh"

#include "G4SDManager.hh"
#include "G4RunManager.hh"

#include "G4ios.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include <stdexcept>
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

using namespace std;
using namespace TrainingDataTypes;

GasChamberSD::GasChamberSD(G4String name, GasMixtureProperties *_gasMixtureProperties, G4int verbose)
    : G4VSensitiveDetector(name), gasMixtureProperties(_gasMixtureProperties),
    fHitsCollection(nullptr), fMessenger(nullptr), fHCID(-1), fEventId(-1), fTrackId(-1),
    fNbOfStepPoints(0),
    hitTupleActivated(true), digiTupleActivated(true)
{
    if(gasMixtureProperties == nullptr)
        throw invalid_argument("In GasChamberSD::GasChamberSD(G4String, GasMixtureProperties *, G4int), a pointer to GasMixtureProperties is a null pointer.");
    verboseLevel = verbose;
    collectionName.insert("GasChamberHColl");
    InitManagers();
    InitDigitizer();
    DefineCommands();
}

void GasChamberSD::InitDigitizer()
{
    auto container = ParamContainerTable::Instance()->GetContainer("gas_chamber");
    padPlaneX = container->GetParamD("chamberX"), padPlaneY = container->GetParamD("chamberY"), padPlaneZ = container->GetParamD("chamberZ");
    padCenterX = container->GetParamD("chamberPosX"), padCenterY = container->GetParamD("chamberPosY"), padCenterZ = container->GetParamD("chamberPosZ");

    // Digitizer    
    digitizer = new GasChamberDigitizer("GasChamberDigitizer");
    digitizer->SetGasMixtureProperties(gasMixtureProperties);
    digitizerManager->AddNewModule(digitizer);
}

void GasChamberSD::InitManagers()
{
    tupleVectorManager = TupleVectorManager::Instance();
    analysisManager = G4AnalysisManager::Instance();
    digitizerManager = G4DigiManager::GetDMpointer();
}

GasChamberSD::~GasChamberSD()
{
    delete fMessenger;
}

void GasChamberSD::Initialize(G4HCofThisEvent *hce)
{
    fHitsCollection = new GasChamberHitsCollection(SensitiveDetectorName, collectionName[0]);
    if(fHCID < 0)
    {
        fHCID = G4SDManager::GetSDMpointer()->GetCollectionID(fHitsCollection);
    }
    hce->AddHitsCollection(fHCID, fHitsCollection);

    flag = 0, Ek = 0.;
    pxv = 0., pyv = 0., pzv = 0.;
    xv = 0., yv = 0., zv = 0.;
    theta = 0., trkLen = 0.;
    secFlags.assign(static_cast<int>(nParticles), static_cast<int>(Empty));

    fNbOfStepPoints = 0;
    fEventId = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
    fTrackId = -1;

    TupleInitializerBase::ActivateTuple("tree_gc1", hitTupleActivated);
    TupleInitializerBase::ActivateTuple("tree_gc2", hitTupleActivated);
    TupleInitializerBase::ActivateTuple("tree_gc3", digiTupleActivated);
    TupleInitializerBase::ActivateTuple("tree_gc4", hitHistActivated);

    digitizer->UpdateGasProperties();
}

G4bool GasChamberSD::ProcessHits(G4Step *step, G4TouchableHistory *)
{
    ProcessTrack(step);
    ProcessSecondaries(step);
    return true;
}

void GasChamberSD::ProcessTrack(const G4Step *step)
{
    const auto track = step->GetTrack();
    const auto pDynamic = step->GetTrack()->GetDynamicParticle();
    const auto postStepPoint = step->GetPostStepPoint();
    // if end or geometry boundary of Oxygen track
    if(pDynamic->GetParticleDefinition()->GetParticleName() == "O16" &&
        (track->GetTrackStatus() == G4TrackStatus::fStopAndKill || postStepPoint->GetStepStatus() == G4StepStatus::fGeomBoundary))
        trkLen = track->GetTrackLength();

    // if new track
    if(fTrackId != track->GetTrackID())
    {
        if(fHitsCollection->GetSize() > 0)
            (*fHitsCollection)[fHitsCollection->GetSize() - 1]->SetNbOfStepPoints(fNbOfStepPoints);
        fNbOfStepPoints = 0;
        fTrackId = track->GetTrackID();
        fHitsCollection->insert(new GasChamberHit(pDynamic, fEventId, fTrackId));
    }
    auto hit = (*fHitsCollection)[fHitsCollection->GetSize() - 1];
    hit->AppendPosition(track->GetPosition());
    hit->AppendMomentum(track->GetMomentum());
    hit->AppendEdep(step->GetTotalEnergyDeposit());
    hit->AppendCharge(pDynamic->GetCharge());
    hit->AppendTime(track->GetGlobalTime());
    hit->AppendStepLen(step->GetStepLength());
    hit->AddEdepSum(step->GetTotalEnergyDeposit());
    hit->AddTrackLength(step->GetStepLength());
    ++fNbOfStepPoints;    
}

void GasChamberSD::ProcessSecondaries(const G4Step* step)
{
    const auto track = step->GetTrack();
    const auto pDynamic = step->GetTrack()->GetDynamicParticle();
    const auto secondaries = step->GetSecondaryInCurrentStep();
    if(secondaries != nullptr)
        for(auto trackSec : *secondaries)
        {
            EParticle parEnum = FromParticleDef(trackSec->GetParticleDefinition());
            switch(parEnum)
            {
                case Oxygen:
                {
                    secFlags[parEnum] = 1;
                    auto pos = track->GetPosition();
                    theta = trackSec->GetMomentumDirection().angle(pDynamic->GetMomentumDirection());
                    Ek = trackSec->GetKineticEnergy();
                    xv = pos.getZ() - padCenterX + padPlaneX/2;
                    yv = pos.getY() - padCenterY + padPlaneY/2;
                    zv = pos.getX() - padCenterZ + padPlaneZ/2;
                    pxv = trackSec->GetMomentum().getZ();
                    pyv = trackSec->GetMomentum().getY();
                    pzv = trackSec->GetMomentum().getX();
                    break;
                }
                default:
                {
                    secFlags[parEnum] = 1;
                    break;
                } 
            }
        }
}

void GasChamberSD::EndOfEvent(G4HCofThisEvent *)
{
    // fill the # of the step points of the last track if more than one.
    if(fHitsCollection->GetSize() > 0)
        (*fHitsCollection)[fHitsCollection->GetSize() - 1]->SetNbOfStepPoints(fNbOfStepPoints);
    if(hitTupleActivated)
        FillHitTuples();
    if(digiTupleActivated)
        FillDigiTuples();
    if(verboseLevel > 0)
        PrintEndOfEvents();
}

void GasChamberSD::FillHitTuples()
{
    // tuple saved by event
    analysisManager->FillNtupleIColumn(0, 0, fHitsCollection->GetSize());
    analysisManager->AddNtupleRow(0);

    // tuple saved by track
    auto tupleVector2 = tupleVectorManager->GetTupleVectorContainer("tree_gc2");
    for(size_t i = 0;i < fHitsCollection->GetSize();++i)
    {
        auto hit = static_cast<GasChamberHit*>(fHitsCollection->GetHit(i));
        analysisManager->FillNtupleIColumn(1, 0, G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID());
        analysisManager->FillNtupleIColumn(1, 1, hit->GetTrackId());
        analysisManager->FillNtupleIColumn(1, 2, hit->GetNbOfStepPoints());
        analysisManager->FillNtupleIColumn(1, 3, hit->GetParticleEnum());
        analysisManager->FillNtupleFColumn(1, 4, hit->GetMass());
        analysisManager->FillNtupleFColumn(1, 5, hit->GetTrackLength());
        analysisManager->FillNtupleFColumn(1, 6, hit->GetEdepSum());

        // vector part
        tupleVector2->FillVectorF("x", hit->GetPosX());
        tupleVector2->FillVectorF("y", hit->GetPosY());
        tupleVector2->FillVectorF("z", hit->GetPosZ());
        tupleVector2->FillVectorF("px", hit->GetMomX());
        tupleVector2->FillVectorF("py", hit->GetMomY());
        tupleVector2->FillVectorF("pz", hit->GetMomZ());
        tupleVector2->FillVectorF("eDep", hit->GetEdep());
        tupleVector2->FillVectorF("q", hit->GetCharge());
        tupleVector2->FillVectorF("t", hit->GetTime());
        tupleVector2->FillVectorF("stepLen", hit->GetStepLen());
        analysisManager->AddNtupleRow(1);
        tupleVector2->ClearVectors();
    }
}

void GasChamberSD::FillDigiTuples()
{
    try {
        // digitization
        auto tupleVector3 = tupleVectorManager->GetTupleVectorContainer("tree_gc3");
        digitizer->FillPadsInfo(fHitsCollection);
        analysisManager->FillNtupleIColumn(2, 0, flag);
        analysisManager->FillNtupleFColumn(2, 1, Ek);
        analysisManager->FillNtupleFColumn(2, 2, pxv);
        analysisManager->FillNtupleFColumn(2, 3, pyv);
        analysisManager->FillNtupleFColumn(2, 4, pzv);
        analysisManager->FillNtupleFColumn(2, 5, xv);
        analysisManager->FillNtupleFColumn(2, 6, yv);
        analysisManager->FillNtupleFColumn(2, 7, zv);
        analysisManager->FillNtupleFColumn(2, 8, theta);
        analysisManager->FillNtupleFColumn(2, 9, trkLen);
        digitizer->FillChargeOnPads(tupleVector3->GetVectorRefF("qdc"));
        digitizer->FillTimeOnPads(tupleVector3->GetVectorRefF("tSig"));
        tupleVector3->FillVectorI("secFlags", secFlags);
        analysisManager->AddNtupleRow(2);
        digitizer->ClearPads();
    }
    catch(Exception const &e)
    {
        e.WarnGeantKernel(FatalException);
    }
}

void GasChamberSD::PrintEndOfEvents()
{
    int prec = G4cout.precision(4);
    G4cout << "--------------------------------------------------------------------------------------------------------------------------------" << G4endl;
    G4cout << std::setw(40) << std::left << "The # of tracks in this event" << " : " << std::setw(10) << std::right << fHitsCollection->GetSize() << G4endl;
    G4cout << "--------------------------------------------------------------------------------------------------------------------------------" << G4endl;
    if(verboseLevel > 1)
    {
        for(size_t i = 0;i < fHitsCollection->GetSize();++i)
        {
            auto hit = static_cast<GasChamberHit *>(fHitsCollection->GetHit(i));
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

void GasChamberSD::DefineCommands()
{
    fMessenger = new G4GenericMessenger(this, "/attpc/gasChamber/", "Gas Chamger SD control");
    // auto fVerboseCmd = fMessenger->DeclareProperty("verbose", verboseLevel, "Set verbosity");
    auto commandHitTuple = fMessenger->DeclareProperty("activateHitTuple", hitTupleActivated, "Activate hit tuple(s)");
    fMessenger->DeclareProperty("verbose", verboseLevel, "Set verbosity");
    commandHitTuple.SetParameterName("hitTupleActivated", true);
    commandHitTuple.SetDefaultValue("true");
    commandHitTuple.SetGuidance("Activate writing hit tuple(s)");

    auto commandDigiTuple = fMessenger->DeclareProperty("activateDigiTuple", digiTupleActivated, "Activate digi tuple(s)");
    commandDigiTuple.SetParameterName("digiTupleActivated", true);
    commandDigiTuple.SetDefaultValue("true");
    commandDigiTuple.SetGuidance("Activate writing digi tuple(s)");
}
