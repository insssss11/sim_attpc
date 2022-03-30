/// \file TupleInitializerGasChamber.cc
/// \brief Implementation of the TupleInitializerGasChamber class

#include "tuple/TupleInitializerGasChamber.hh"

TupleInitializerGasChamber::TupleInitializerGasChamber() : TupleInitializerBase("gas_chamber")
{
    try {
        tupleVectorManager->AddTupleVectorContainer("tree_gc2")
            ->AddVectorsF("x", "y", "z", "px", "py", "pz", "eDep", "t", "q", "stepLen");
        tupleVectorManager->AddTupleVectorContainer("tree_gc3")
            ->AddVectorsF("qdc");
    }
    catch(Exception const &e)
    {
        G4cerr << "Faied to add a tuple vector container for TupleInitializerGasChamber class" << G4endl;
        e.WarnGeantKernel(JustWarning);
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
        analysisManager->CreateNtupleFColumn("mass"); // 1 4
        analysisManager->CreateNtupleFColumn("trkLen"); // 1 5
        analysisManager->CreateNtupleFColumn("eDepSum"); // 1 6
        // analysisManager->CreateNtupleSColumn("part"); // 1 7

        // vector part
        auto tupleVector2 = tupleVectorManager->GetTupleVectorContainer("tree_gc2");
        analysisManager->CreateNtupleFColumn("x", tupleVector2->GetVectorRefF("x"));
        analysisManager->CreateNtupleFColumn("y", tupleVector2->GetVectorRefF("y"));
        analysisManager->CreateNtupleFColumn("z", tupleVector2->GetVectorRefF("z"));
        analysisManager->CreateNtupleFColumn("px", tupleVector2->GetVectorRefF("px"));
        analysisManager->CreateNtupleFColumn("py", tupleVector2->GetVectorRefF("py"));
        analysisManager->CreateNtupleFColumn("pz", tupleVector2->GetVectorRefF("pz"));
        analysisManager->CreateNtupleFColumn("eDep", tupleVector2->GetVectorRefF("eDep"));
        // analysisManager->CreateNtupleFColumn("t", tupleVector2->GetVectorRefF("t"));
        // analysisManager->CreateNtupleFColumn("q", tupleVector2->GetVectorRefF("q"));
        analysisManager->CreateNtupleFColumn("stepLen", tupleVector2->GetVectorRefF("stepLen"));
        analysisManager->FinishNtuple();
    }
    catch(Exception const &e)
    {
        G4cerr << "Faied to create Ntuples to the analysis manager in TupleInitializerGasChamber::InitHitTuples()" << G4endl;
        e.WarnGeantKernel(FatalException);
    }
}

void TupleInitializerGasChamber::InitDigiTuples()
{
    try {
        auto tupleVector3 = tupleVectorManager->GetTupleVectorContainer("tree_gc3");
        analysisManager->CreateNtuple("tree_gc3", "gas chamber digitization data");
        analysisManager->CreateNtupleFColumn("qdc", tupleVector3->GetVectorRefF("qdc"));
        analysisManager->FinishNtuple();
    }
    catch(Exception const &e)
    {
        G4cerr << "Faied to create Ntuples to the analysis manager in TupleInitializerGasChamber::InitHitTuples()" << G4endl;
        e.WarnGeantKernel(FatalException);
    }
}
