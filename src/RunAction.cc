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
    fAnalysisManager = G4AnalysisManager::Instance();
    fAnalysisManager->SetFileName("");
    // If running in MT, merge all tuples after the end of run.
    if(G4RunManager::GetRunManager()->GetNumberOfThreads() > 1)
        fAnalysisManager->SetNtupleMerging(true);
        
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
    // creating ntuples
    CreateTuplesGasChamber();
    fAnalysisManager->FinishNtuple();

    fAnalysisManager->SetActivation(fAnaActivated);
    // if output file name has been changed, open new file.
    if(fFileName != fAnalysisManager->GetFileName())
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
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::DefineCommands()
{
    fMessenger = new G4GenericMessenger(this, "/attpc/output/", "File output control");
    fMessenger->DeclareProperty("fileName", fFileName, "Output file name");
    fMessenger->DeclareProperty("activate", fAnaActivated, "Activation of file output");
}