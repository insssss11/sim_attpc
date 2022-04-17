/// \file RunAction.cc
/// \brief Implementation of the RunAction class

#include "RunAction.hh"
#include "EventAction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

using namespace std;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::RunAction(EventAction *eventAction)
    : G4UserRunAction(),
    fAnaActivated(false), fFileName("sim_attpc.root"),
    fEventAction(eventAction), fAnalysisManager(nullptr),
    messenger(new RunActionMessenger(this)),
    merger(new TreeMerger),
    tupleInitializer(new TupleInitializer)
{
    merger->AddTreeNameTitle("tree_gc1", "gas chamber hit data saved by event");
    merger->AddTreeNameTitle("tree_gc2", "gas chamber hit data saved by trk");
    merger->AddTreeNameTitle("tree_gc3", "gas chamber digitization data");
    fAnalysisManager = G4AnalysisManager::Instance();
    fAnalysisManager->SetVerboseLevel(1);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunAction::~RunAction()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunAction::BeginOfRunAction(const G4Run * /*run*/)
{
    fAnalysisManager->SetFileName(fFileName);
    fAnalysisManager->OpenFile();
    tupleInitializer->Init();
}

void RunAction::EndOfRunAction(const G4Run * /*run*/)
{
    if(fAnaActivated)
        fAnalysisManager->Write();
    fAnalysisManager->CloseFile();
    fAnalysisManager->Clear();
    if(fAnaActivated && G4RunManager::GetRunManager()->GetRunManagerType() == G4RunManager::RMType::masterRM)
        MergeThreadTrees();
}

void RunAction::MergeThreadTrees()
{
    string filePrefix = fFileName.substr(0, fFileName.find_last_of('.'));
    vector<string> subFiles;
    for(G4int thread = 0;thread < G4RunManager::GetRunManager()->GetNumberOfThreads();++thread)
        subFiles.push_back(filePrefix + "_t" + to_string(thread) + ".root");
    merger->MergeRootFiles(fFileName, subFiles);
}

void RunAction::SetFileName(const std::string &fileName)
{
    fFileName = fileName;
}

void RunAction::MergeFiles(const std::string &_fileName, const std::vector<std::string> &subFiles)
{
    merger->MergeRootFiles(_fileName, subFiles);
}

void RunAction::Activate(G4bool activate)
{
    fAnaActivated = activate;
}
