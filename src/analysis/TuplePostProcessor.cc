/// \file TuplePostProcessor.cc
/// \brief Implementation of the TuplePostProcessor class

#include "analysis/TuplePostProcessor.hh"

#include <string.h>

#include "G4ios.hh"
#include "G4Exception.hh"
#include "g4root_defs.hh"

TuplePostProcessor::TuplePostProcessor(const char *fileName)
    :fRootFile(nullptr)
{
    strncpy(fFileName, fileName, 256 - 1);
}

TuplePostProcessor::~TuplePostProcessor()
{
    if(fRootFile != nullptr)
    {
        if(fRootFile->IsOpen())
            fRootFile->Close();
        delete fRootFile;
    }
}

void TuplePostProcessor::PostProcessTuples()
{
    fRootFile = TFile::Open(fFileName, "UPDATE");
    if(!fRootFile->IsOpen())
    {
        std::ostringstream message;
        message << "Cannot open root file " << fFileName << " to be postprocessed.";
        G4Exception("TuplePostProcessor::PostProcessTuples()",
            "TuplePosPro0000", JustWarning, message);
        return;
    }

    // postprocess tuples of all detector
    PostProcessTuplesGasChamber();

    fRootFile->Write();
}

void TuplePostProcessor::Close()
{
    if(fRootFile != nullptr)
    {
        if(fRootFile->IsOpen())
            fRootFile->Close();
    }
}

void TuplePostProcessor::PostProcessTuplesGasChamber()
{
    static constexpr G4int kMaxNbOfSteps = 5000;

    // tuples from Analysis Manager
    auto tuple_gc1 = (TTree *)fRootFile->Get("tree_gc1");
    auto tuple_gc2 = (TTree *)fRootFile->Get("tree_gc2");
    auto tuple_gc3 = (TTree *)fRootFile->Get("tree_gc3");
    auto tree_gc = new TTree("tree_gc", "tree for gas chamber");

    //------------------------------------------------------------
    // Preparation of Data Reading
    // by events
    G4int Ntrk;
    tuple_gc1->SetBranchAddress("Ntrk", &Ntrk);

    // by tracks
    G4int evtId, trkId, Nstep, atomNum;
    G4double mass, trkLen, eDepSum;
    char part[256];
    tuple_gc2->SetBranchAddress("evtId", &evtId);
    tuple_gc2->SetBranchAddress("trkId", &trkId);
    tuple_gc2->SetBranchAddress("Nstep", &Nstep);
    tuple_gc2->SetBranchAddress("atomNum", &atomNum);
    tuple_gc2->SetBranchAddress("mass", &mass);
    tuple_gc2->SetBranchAddress("trkLen", &trkLen);
    tuple_gc2->SetBranchAddress("eDepSum", &eDepSum);
    // tuple_gc2->SetBranchAddress("part", &trkId);

    // by steps
    G4double posX, posY, posZ;
    G4double momX, momY, momZ;
    G4double time, charge, eDep;
    tuple_gc3->SetBranchAddress("x", &posX);
    tuple_gc3->SetBranchAddress("y", &posY);
    tuple_gc3->SetBranchAddress("z", &posZ);
    tuple_gc3->SetBranchAddress("px", &momX);
    tuple_gc3->SetBranchAddress("py", &momY);
    tuple_gc3->SetBranchAddress("pz", &momZ);
    tuple_gc3->SetBranchAddress("eDep", &eDep);
    tuple_gc3->SetBranchAddress("t", &time);
    tuple_gc3->SetBranchAddress("q", &charge);

    //------------------------------------------------------------
    // Preparation of Data Writing
    G4double posXarr[kMaxNbOfSteps], posYarr[kMaxNbOfSteps], posZarr[kMaxNbOfSteps];
    G4double momXarr[kMaxNbOfSteps], momYarr[kMaxNbOfSteps], momZarr[kMaxNbOfSteps];
    G4double timeArr[kMaxNbOfSteps], chargeArr[kMaxNbOfSteps], eDepArr[kMaxNbOfSteps];
    // tree_gc->Branch("Ntrk", &Ntrk, "Ntrk/I");
    tree_gc->Branch("evtId", &evtId, "evtId/I");
    tree_gc->Branch("trkId", &trkId, "trkId/I");
    tree_gc->Branch("Nstep", &Nstep, "Nstep/I");
    tree_gc->Branch("atomNum", &atomNum);
    tree_gc->Branch("mass", &mass);
    tree_gc->Branch("trkLen", &trkLen);
    tree_gc->Branch("eDepSum", &eDepSum);
    tree_gc->Branch("part", part, "part/C");
    tree_gc->Branch("x", posXarr, "x[Nstep]/D");
    tree_gc->Branch("y", posYarr, "y[Nstep]/D");
    tree_gc->Branch("z", posZarr, "z[Nstep]/D");
    tree_gc->Branch("px", momXarr, "px[Nstep]/D");
    tree_gc->Branch("py", momYarr, "py[Nstep]/D");
    tree_gc->Branch("pz", momZarr, "pz[Nstep]/D");
    tree_gc->Branch("eDep", eDepArr, "eDep[Nstep]/D");
    tree_gc->Branch("t", timeArr, "t[Nstep]/D");
    tree_gc->Branch("q", chargeArr, "q[Nstep]/D");

    Long64_t trkIdx = 0, stepIdx = 0;

    // for-loop
    for(Long64_t evt = 0;evt < tuple_gc1->GetEntries();++evt)
    {
        tuple_gc1->GetEntry(evt);
        for(Long64_t trk = 0;trk < Ntrk;++trk)
        {
            tuple_gc2->GetEntry(trkIdx);
            ++trkIdx;
            for(Long64_t step = 0;step < Nstep;++step)
            {
                tuple_gc3->GetEntry(stepIdx);
                ++stepIdx;
                posXarr[step] = posX;
                posYarr[step] = posY;
                posZarr[step] = posZ;
                momXarr[step] = momX;
                momYarr[step] = momY;
                momZarr[step] = momZ;
                eDepArr[step] = eDep;
                timeArr[step] = time;
                chargeArr[step] = charge;
            }
            tree_gc->Fill();
        }
    }
    gDirectory->Delete("tree_gc1;*");
    gDirectory->Delete("tree_gc2;*");
    gDirectory->Delete("tree_gc3;*");
}



