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
    fEventAction(eventAction), fAnalysisManager(nullptr),
    tupleInitializer(new TupleInitializer)
{
    fAnalysisManager = G4AnalysisManager::Instance();
    fAnalysisManager->SetVerboseLevel(1);
    // If running in MT, merge all tuples after the end of run.
    if(G4RunManager::GetRunManager()->GetRunManagerType() != G4RunManager::RMType::sequentialRM)
        fAnalysisManager->SetNtupleMerging(true);
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
    fAnalysisManager->SetFileName(fFileName);
    fAnalysisManager->OpenFile();
    tupleInitializer->Init();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run * /*run*/)
{
    if(fAnaActivated)
        fAnalysisManager->Write();
    fAnalysisManager->CloseFile();
    fAnalysisManager->Clear();
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