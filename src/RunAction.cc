/// \file RunAction.cc
/// \brief Implementation of the RunAction class

#include "RunAction.hh"
#include "EventAction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction(EventAction *eventAction)
    : G4UserRunAction(),
    fAnaActivated(false), fFileName("sim_attpc.root"),
    fEventAction(eventAction), fAnalysisManager(nullptr)
{
    // it is recommened that analysis manager instance be created in user run action constructor.
    fAnalysisManager = G4AnalysisManager::Instance();
    fAnalysisManager->SetVerboseLevel(1);
    // If running in MT, merge all tuples after the end of run.
    fAnalysisManager->SetNtupleMerging(true);
    
    // creating ntuples
    // why tuples must be created in constructor of user RunAction class, not in RunAction::BeginOfRunAction?
    CreateTuplesGasChamber();
    fAnalysisManager->FinishNtuple();

    DefineCommands();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::~RunAction()
{
    delete fMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run * /*run*/)
{
    fAnalysisManager->SetActivation(fAnaActivated);
    fAnalysisManager->OpenFile(fFileName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run * /*run*/)
{
    // save histograms & ntuple
    //
    if(fAnalysisManager->GetActivation())
        fAnalysisManager->Write();
    fAnalysisManager->CloseFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::CreateTuplesGasChamber()
{
    fAnalysisManager->CreateNtuple("tree_gc1", "gas chamber hit data saved by event");
    fAnalysisManager->CreateNtupleIColumn("Ntrk"); // 0 0

    fAnalysisManager->CreateNtuple("tree_gc2", "gas chamber hit data saved by trk");
    fAnalysisManager->CreateNtupleIColumn("evtId"); // 1 0
    fAnalysisManager->CreateNtupleIColumn("trkId"); // 1 1
    fAnalysisManager->CreateNtupleIColumn("Nstep"); // 1 2
    fAnalysisManager->CreateNtupleIColumn("atomNum"); // 1 3
    fAnalysisManager->CreateNtupleDColumn("mass"); // 1 4
    fAnalysisManager->CreateNtupleDColumn("trkLen"); // 1 5
    fAnalysisManager->CreateNtupleDColumn("eDepSum"); // 1 6
    // fAnalysisManager->CreateNtupleSColumn("part"); // 1 7
    
    // vector part
    fAnalysisManager->CreateNtupleDColumn("x", *fEventAction->GetVectorPtrD("tree_gc2", "x"));
    fAnalysisManager->CreateNtupleDColumn("y", *fEventAction->GetVectorPtrD("tree_gc2", "y"));
    fAnalysisManager->CreateNtupleDColumn("z", *fEventAction->GetVectorPtrD("tree_gc2", "z"));
    fAnalysisManager->CreateNtupleDColumn("px", *fEventAction->GetVectorPtrD("tree_gc2", "px"));
    fAnalysisManager->CreateNtupleDColumn("py", *fEventAction->GetVectorPtrD("tree_gc2", "py"));
    fAnalysisManager->CreateNtupleDColumn("pz", *fEventAction->GetVectorPtrD("tree_gc2", "pz"));
    fAnalysisManager->CreateNtupleDColumn("eDep", *fEventAction->GetVectorPtrD("tree_gc2", "eDep"));
    // fAnalysisManager->CreateNtupleDColumn("t", *fEventAction->GetVectorPtrD("tree_gc2", "t"));
    // fAnalysisManager->CreateNtupleDColumn("q", *fEventAction->GetVectorPtrD("tree_gc2", "q"));
    fAnalysisManager->CreateNtupleDColumn("stepLen", *fEventAction->GetVectorPtrD("tree_gc2", "stepLen"));

    // tuples for digitizer
    fAnalysisManager->CreateNtuple("tree_gc3", "gas chamber digitization data");
    fAnalysisManager->CreateNtupleDColumn("qdc", *fEventAction->GetVectorPtrD("tree_gc3", "qdc"));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::DefineCommands()
{
    fMessenger = new G4GenericMessenger(this, "/attpc/output/", "File output control");
    
    auto activateCmd = fMessenger->DeclareProperty("activate", fAnaActivated, "Activation of file output");
    activateCmd.SetParameterName("active", true);
    activateCmd.SetDefaultValue("true");

    fMessenger->DeclareProperty("setFileName", fFileName, "Set Name of output file.");
}