/// \file RunAction.cc
/// \brief Implementation of the RunAction class

#include "tuple/TupleInitializerBase.hh"
#include "tuple/TupleVectorManager.hh"

#include "RunAction.hh"
#include "EventAction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"

#include <fstream>

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
    WriteHitHistTxt();
}

void RunAction::WriteHitHistTxt()
{
    vector<G4int> &hits = TupleVectorManager::Instance()->GetTupleVectorContainer("tree_gc4")->GetVectorRefI("hits");
    static vector<atomic<G4int> > hitHistSum(hits.size());

    if(fAnaActivated)
    {
        // consider situation under multi-threading mode.
        // All hit hist data of worker thread are summed up and saved in master thread.
        G4RunManager::RMType runManagerType = G4RunManager::GetRunManager()->GetRunManagerType();
        if(runManagerType == G4RunManager::RMType::workerRM)
            AddHitHist(hitHistSum, hits);
        else if(runManagerType == G4RunManager::RMType::masterRM)
            SaveHitHist(hitHistSum);
        else if(runManagerType == G4RunManager::RMType::sequentialRM)
        {
            AddHitHist(hitHistSum, hits);
            SaveHitHist(hitHistSum);
        }
    }
}

void RunAction::AddHitHist(vector<atomic<G4int> > &hitHistSum, const vector<G4int> &hitHist)
{
    for(size_t i = 0;i < hitHist.size();++i)
        hitHistSum.at(i).fetch_add(hitHist.at(i));
}

void RunAction::SaveHitHist(const vector<atomic<G4int> > &hitHistSum) const
{
    const string &histFileName = fFileName.substr(0, fFileName.rfind(".root")) + "_hit_hist.txt";
    ofstream histTxt(histFileName, ios_base::out);
    for(auto &hit : hitHistSum)
        histTxt << hit.load() << " ";
    histTxt << G4endl;
    histTxt.close();
    G4cout << "Text data file for Hit Histgram are successfully saved : " + histFileName << G4endl;
}


void RunAction::MergeThreadTrees()
{
    merger->ClearTreeNameTitles();
    for(auto stat : TupleInitializerBase::GetTuplesActivationStat())
        if(stat.second.second)
            merger->AddTreeNameTitle(stat.first, stat.second.first);
    string filePrefix = fFileName.substr(0, fFileName.find_last_of('.'));
    vector<string> subFiles;
    for(G4int thread = 0;thread < G4RunManager::GetRunManager()->GetNumberOfThreads();++thread)
        subFiles.push_back(filePrefix + "_t" + to_string(thread) + ".root");
    merger->MergeRootFiles(fFileName, subFiles);
}

void RunAction::SetFileName(const string &fileName)
{
    fFileName = fileName;
}

void RunAction::MergeFiles(const string &_fileName, const vector<string> &subFiles)
{
    merger->MergeRootFiles(_fileName, subFiles);
}

void RunAction::Activate(G4bool activate)
{
    fAnaActivated = activate;
}
