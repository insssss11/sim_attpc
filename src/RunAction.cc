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
    
    // If running in MT, merge all tuples after the end of run.
    if(G4RunManager::GetRunManager()->GetNumberOfThreads() > 1)
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
    delete fAnalysisManager;
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
    fAnalysisManager->CreateNtupleIColumn("Nstp"); // 1 1
    fAnalysisManager->CreateNtupleIColumn("Z"); // 1 2
    fAnalysisManager->CreateNtupleDColumn("mass"); // 1 3
    fAnalysisManager->CreateNtupleDColumn("Ltrk"); // 1 4
    fAnalysisManager->CreateNtupleDColumn("EdepSum"); // 1 5
    fAnalysisManager->CreateNtupleSColumn("part"); // 1 6

    fAnalysisManager->CreateNtuple("tree_gc3", "gas chamber hit data saved by steps");
    fAnalysisManager->CreateNtupleDColumn("x"); // 2 0
    fAnalysisManager->CreateNtupleDColumn("y"); // 2 1
    fAnalysisManager->CreateNtupleDColumn("z"); // 2 2
    fAnalysisManager->CreateNtupleDColumn("px"); // 2 3
    fAnalysisManager->CreateNtupleDColumn("py"); // 2 4
    fAnalysisManager->CreateNtupleDColumn("pz"); // 2 5
    fAnalysisManager->CreateNtupleDColumn("Edep"); // 2 6
    fAnalysisManager->CreateNtupleDColumn("t"); // 2 7
    fAnalysisManager->CreateNtupleDColumn("q"); // 2 8
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::DefineCommands()
{
    fMessenger = new G4GenericMessenger(this, "/attpc/output/", "File output control");
    fMessenger->DeclareProperty("fileName", fFileName, "Output file name");
    fMessenger->DeclareProperty("activate", fAnaActivated, "Activation of file output");
}