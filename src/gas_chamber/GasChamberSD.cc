/// \file GasChamberSD.cc
/// \brief Implementation of the GasChamberSD class

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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GasChamberSD::GasChamberSD(G4String name, G4int verbose)
    : G4VSensitiveDetector(name),
    fHitsCollection(nullptr), fMessenger(nullptr), fHCID(-1), fEventId(-1), fTrackId(-1),
    fNbOfStepPoints(0),
    hitTupleActivated(true), digiTupleActivated(true)
{
    verboseLevel = verbose;
    collectionName.insert("GasChamberHColl");
    InitManagers();
    InitDigitizer();
    InitTuples();
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
    fEventId = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
    fTrackId = -1;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool GasChamberSD::ProcessHits(G4Step *step, G4TouchableHistory *)
{
    const auto track = step->GetTrack();
    const auto pDynamic = step->GetTrack()->GetDynamicParticle();
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
    return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberSD::InitManagers()
{
    tupleVectorManager = TupleVectorManager::Instance();
    analysisManager = G4AnalysisManager::Instance();
    digitizerManager = G4DigiManager::GetDMpointer();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberSD::InitDigitizer()
{
    auto container = ParamContainerTable::Instance()->GetContainer("gas_chamber");
    // Digitizer
    digitizer = new GasChamberDigitizer(
        "GasChamberDigitizer", container->GetParamD("chamberX"), container->GetParamD("chamberY"),
        container->GetParamI("nPadX"), container->GetParamI("nPadY"),
        container->GetParamD("W"),
        G4ThreeVector(container->GetParamD("chamberPosX"), container->GetParamD("chamberPosY"), container->GetParamD("chamberPosZ")));
    digitizer->SetPadMargin(container->GetParamD("margin"));
    digitizer->SetChargeMultiplication(container->GetParamD("multiplication"));
    digitizerManager->AddNewModule(digitizer);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberSD::InitTuples()
{
    InitHitTupleVectors();
    InitHitTuples();

    InitDigiTupleVectors();
    InitDigiTuples();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberSD::InitHitTupleVectors()
{
    tupleVectorManager->AddTupleVectorContainer("tree_gc2")
        ->AddVectorsD("x", "y", "z", "px", "py", "pz", "eDep", "t", "q", "stepLen");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberSD::InitHitTuples()
{
    analysisManager->CreateNtuple("tree_gc1", "gas chamber hit data saved by event");
    analysisManager->CreateNtupleIColumn("Ntrk"); // 0 0

    analysisManager->CreateNtuple("tree_gc2", "gas chamber hit data saved by trk");
    analysisManager->CreateNtupleIColumn("evtId"); // 1 0
    analysisManager->CreateNtupleIColumn("trkId"); // 1 1
    analysisManager->CreateNtupleIColumn("Nstep"); // 1 2
    analysisManager->CreateNtupleIColumn("atomNum"); // 1 3
    analysisManager->CreateNtupleDColumn("mass"); // 1 4
    analysisManager->CreateNtupleDColumn("trkLen"); // 1 5
    analysisManager->CreateNtupleDColumn("eDepSum"); // 1 6
    // analysisManager->CreateNtupleSColumn("part"); // 1 7

    // vector part
    auto tupleVector2 = tupleVectorManager->GetTupleVectorContainer("tree_gc2");
    analysisManager->CreateNtupleDColumn("x", tupleVector2->GetVectorRefD("x"));
    analysisManager->CreateNtupleDColumn("y", tupleVector2->GetVectorRefD("y"));
    analysisManager->CreateNtupleDColumn("z", tupleVector2->GetVectorRefD("z"));
    analysisManager->CreateNtupleDColumn("px", tupleVector2->GetVectorRefD("px"));
    analysisManager->CreateNtupleDColumn("py", tupleVector2->GetVectorRefD("py"));
    analysisManager->CreateNtupleDColumn("pz", tupleVector2->GetVectorRefD("pz"));
    analysisManager->CreateNtupleDColumn("eDep", tupleVector2->GetVectorRefD("eDep"));
    // analysisManager->CreateNtupleDColumn("t", tupleVector2->GetVectorRefD("t"));
    // analysisManager->CreateNtupleDColumn("q", tupleVector2->GetVectorRefD("q"));
    analysisManager->CreateNtupleDColumn("stepLen", tupleVector2->GetVectorRefD("stepLen"));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberSD::InitDigiTupleVectors()
{
    tupleVectorManager->AddTupleVectorContainer("tree_gc3")
        ->AddVectorsD("qdc");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberSD::InitDigiTuples()
{
    auto tupleVector3 = tupleVectorManager->GetTupleVectorContainer("tree_gc3");
    // tuples for digitizer
    analysisManager->CreateNtuple("tree_gc3", "gas chamber digitization data");
    analysisManager->CreateNtupleDColumn("qdc", tupleVector3->GetVectorRefD("qdc"));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

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
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberSD::FillDigiTuples()
{
    // digitization
    auto tupleVector3 = tupleVectorManager->GetTupleVectorContainer("tree_gc3");
    digitizer->FillPadsInfo(fHitsCollection);
    tupleVector3->FillVectorD("qdc", digitizer->GetChargeOnPads());
    analysisManager->AddNtupleRow(2);
    tupleVector3->ClearVectors();
    digitizer->ClearPads();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

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

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberSD::DefineCommands()
{
    fMessenger = new G4GenericMessenger(this, "/attpc/gasChamber/", "Gas Chamger SD control");
    // auto fVerboseCmd = fMessenger->DeclareProperty("verbose", verboseLevel, "Set verbosity");
    fMessenger->DeclareProperty("verbose", verboseLevel, "Set verbosity");
    auto commandHitTuple
        = fMessenger->DeclareProperty("activateHitTuple", hitTupleActivated, "Activate hit tuple(s)");
    commandHitTuple.SetParameterName("hitTupleActivated", true);
    commandHitTuple.SetDefaultValue("true");
    commandHitTuple.SetGuidance("Activate writing hit tuple(s)");
    auto commandDigiTuple = 
        fMessenger->DeclareProperty("activateDigiTuple", digiTupleActivated, "Activate digi tuple(s)");
    commandDigiTuple.SetParameterName("digiTupleActivated", true);
    commandDigiTuple.SetDefaultValue("true");
    commandDigiTuple.SetGuidance("Activate writing digi tuple(s)");    
}