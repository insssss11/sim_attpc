/// \file TupleInitializerGasChamber.cc
/// \brief Implementation of the TupleInitializerGasChamber class

#include "tuple/TupleInitializerGasChamber.hh"

TupleInitializerGasChamber::TupleInitializerGasChamber() : TupleInitializerBase("gas_chamber")
{
    try {
        tupleVectorManager->AddTupleVectorContainer("tree_gc2")
            ->AddVectorsD("x", "y", "z", "px", "py", "pz", "eDep", "t", "q", "stepLen");
        tupleVectorManager->AddTupleVectorContainer("tree_gc3")
            ->AddVectorsD("qdc");
    }
    catch(Exception const &e)
    {
        G4cerr << "Faied to add a tuple vector container for TupleInitializerGasChamber class" << G4endl;
        e.WarnGeantKernel();
    }
}

TupleInitializerGasChamber::~TupleInitializerGasChamber()
{
}

void TupleInitializerGasChamber::Init()
{
    InitHitTuples();
    InitDigiTuples();
}

void TupleInitializerGasChamber::InitHitTuples()
{
    try {
        analysisManager->CreateNtuple("tree_gc1", "gas chamber hit data saved by event");
        analysisManager->CreateNtupleIColumn("Ntrk"); // 0 0
        analysisManager->FinishNtuple();

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
        analysisManager->FinishNtuple();
    }
    catch(Exception const &e)
    {
        G4cerr << "Faied to create Ntuples to the analysis manager in TupleInitializerGasChamber::InitHitTuples()" << G4endl;
        e.WarnGeantKernel();
    }
}

void TupleInitializerGasChamber::InitDigiTuples()
{
    try {
        auto tupleVector3 = tupleVectorManager->GetTupleVectorContainer("tree_gc3");
        analysisManager->CreateNtuple("tree_gc3", "gas chamber digitization data");
        analysisManager->CreateNtupleDColumn("qdc", tupleVector3->GetVectorRefD("qdc"));
        analysisManager->FinishNtuple();
    }
    catch(Exception const &e)
    {
        G4cerr << "Faied to create Ntuples to the analysis manager in TupleInitializerGasChamber::InitHitTuples()" << G4endl;
        e.WarnGeantKernel();
    }
}
