/// \file RunAction.cc
/// \brief Implementation of the RunAction class

#include "RunAction.hh"
#include "EventAction.hh"

#include "G4Run.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

using G4AnalysisManager = G4GenericAnalysisManager;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction(EventAction *eventAction)
    : G4UserRunAction(),
    fEventAction(eventAction), fAnalysisManager(nullptr)
{
    fAnalysisManager = G4AnalysisManager::Instance();
    fAnalysisManager->SetNtupleMerging(true);
    fAnalysisManager->SetFileName("");
    fAnalysisManager->SetDefaultFileType("root");
    /*
    // Create the generic analysis manager
    // The choice of analysis technology is done according to the file extension
    auto analysisManager = G4AnalysisManager::Instance();
    analysisManager->SetVerboseLevel(1);

    // Default settings
    analysisManager->SetNtupleMerging(true);
    // Note: merging ntuples is available only with Root output
    analysisManager->SetFileName("");
    // If the filename extension is not provided, the default file type (root)
    // will be used for all files specified without extension.
  // analysisManager->SetDefaultFileType("xml");
     // The default file type (root) can be redefined by the user.

  // Book histograms, ntuple

  // Creating 1D histograms
    analysisManager
      ->CreateH1("Chamber1", "Drift Chamber 1 # Hits", 50, 0., 50); // h1 Id = 0
    analysisManager
      ->CreateH1("Chamber2", "Drift Chamber 2 # Hits", 50, 0., 50); // h1 Id = 1

    // Creating 2D histograms
    analysisManager
      ->CreateH2("Chamber1 XY", "Drift Chamber 1 X vs Y",           // h2 Id = 0
        50, -1000., 1000, 50, -300., 300.);
    analysisManager
      ->CreateH2("Chamber2 XY", "Drift Chamber 2 X vs Y",           // h2 Id = 1
        50, -1500., 1500, 50, -300., 300.);

    // Creating ntuple
    if (fEventAction)
    {
      analysisManager->CreateNtuple("", "Hits");
      analysisManager->CreateNtupleIColumn("Dc1Hits");  // column Id = 0
      analysisManager->CreateNtupleIColumn("Dc2Hits");  // column Id = 1
      analysisManager->CreateNtupleDColumn("ECEnergy"); // column Id = 2
      analysisManager->CreateNtupleDColumn("HCEnergy"); // column Id = 3
      analysisManager->CreateNtupleDColumn("Time1");    // column Id = 4
      analysisManager->CreateNtupleDColumn("Time2");    // column Id = 5
      analysisManager                                   // column Id = 6
        ->CreateNtupleDColumn("ECEnergyVector", fEventAction->GetEmCalEdep());
      analysisManager                                   // column Id = 7
        ->CreateNtupleDColumn("HCEnergyVector", fEventAction->GetHadCalEdep());
      analysisManager->FinishNtuple();
    }

    // Set ntuple output file
    analysisManager->SetNtupleFileName(0, "B4ntuple");
    */
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::~RunAction()
{
    delete fAnalysisManager;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run * /*run*/)
{
    // fAnalysisManager->OpenFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::EndOfRunAction(const G4Run * /*run*/)
{
    /*
    // save histograms & ntuple
    //
    fAnalysisManager->Write();
    fAnalysisManager->CloseFile();
    */
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
