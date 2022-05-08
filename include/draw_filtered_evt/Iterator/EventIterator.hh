#ifndef EventIterator_h
#define EventIterator_h 1

#include <vector>
#include <fstream>

#include "draw_filtered_evt/Iterator/IIterator.hh"
#include "draw_filtered_evt/PadEvent.hh"

#include "training_data/TrainingDataTypes.hh"

#include "TFile.h"
#include "TTree.h"

class EventIterator : public IIterator<PadEvent>
{
    public:
    EventIterator(const std::string &item, const std::string &bg)
    {
        Reset();
        qdc = new std::vector<float>;
        tSig = new std::vector<float>;
        x = new std::vector<float>;
        y = new std::vector<float>;
        secFlags = new std::vector<int>;
        parEnums = new std::vector<TrainingDataTypes::EParticle>;

        fileEvt = TFile::Open(item.c_str(), "READ");
        fileBg = TFile::Open(bg.c_str(), "READ");
        treeEvt1 = (TTree*)fileEvt->Get("tree_gc1");
        treeEvt2 = (TTree*)fileEvt->Get("tree_gc2");
        treeEvt3 = (TTree*)fileEvt->Get("tree_gc3");
        treeBg1 = (TTree*)fileBg->Get("tree_gc1");
        treeBg2 = (TTree*)fileBg->Get("tree_gc2");
        treeBg3 = (TTree*)fileBg->Get("tree_gc3");
        entries = treeEvt1->GetEntries();
        
        treeEvt1->SetBranchAddress("Ntrk", &nTrk);
        treeEvt2->SetBranchAddress("parEnum", &parEnum);
        treeEvt2->SetBranchAddress("z", &x);
        treeEvt2->SetBranchAddress("y", &y);
        treeEvt2->SetBranchAddress("evtId", &evtId);
        treeEvt3->SetBranchAddress("qdc", &qdc);
        treeEvt3->SetBranchAddress("tSig", &tSig);
        treeEvt3->SetBranchAddress("secFlags", &secFlags);

        treeBg1->SetBranchAddress("Ntrk", &nTrk);
        treeBg2->SetBranchAddress("parEnum", &parEnum);
        treeBg2->SetBranchAddress("z", &x);
        treeBg2->SetBranchAddress("y", &y);
        treeBg2->SetBranchAddress("evtId", &evtId);
        treeBg3->SetBranchAddress("qdc", &qdc);
        treeBg3->SetBranchAddress("tSig", &tSig);
        treeBg3->SetBranchAddress("secFlags", &secFlags);
    }

    virtual void Reset() override
    {
        curEntry = 0;
        curEntryEvt2 = 0;
        curEntryBg2 = 0;
    }

    virtual ~EventIterator()
    {
        delete qdc;
        delete tSig;
        delete x;
        delete y;
        delete secFlags;
        delete parEnums;
    }

    virtual bool Next() override
    {
        if(curEntry == entries)
            return false;
        ResetEvent();

        AddEvent(treeEvt1, treeEvt2, treeEvt3, curEntryEvt2);
        AddEvent(treeBg1, treeBg2, treeBg3, curEntryBg2);
        ++curEntry;
        return true;
    }

    private:
    void ResetEvent()
    {
        item.secFlags.assign(static_cast<size_t>(TrainingDataTypes::nParticles), 0);
        item.qdc.clear();
        item.tSig.clear();
        
        item.parEnums.clear();
        item.x.clear();
        item.y.clear();
    }

    void AddEvent(TTree *tree1, TTree *tree2, TTree *tree3, long long &curEntry2)
    {
        tree1->GetEntry(curEntry);
        tree3->GetEntry(curEntry);
        curEvtId = -1;

        // data associated with tree_gc2
        for(long long trkId = 0;trkId < nTrk;++trkId)
        {
            tree2->GetEntry(curEntry2);
            if(trkId == 0)
                curEvtId = evtId;
            else if(curEvtId != evtId)
            {
                std::cerr << "EventId mismatch : " << curEntry << "    " << curEvtId << "    " << evtId << std::endl;
                break;
            }
            item.parEnums.push_back((TrainingDataTypes::EParticle)parEnum);
            item.x.emplace_back(*x);
            item.y.emplace_back(*y);
            ++curEntry2;
        }

        
        // data associated with tree_gc3
        if(item.qdc.empty())
            item.qdc = *qdc;
        else
            for(size_t i = 0;i < qdc->size();++i)
            {
                item.qdc[i] += qdc->at(i);
            }
        for(int i = 0;i < static_cast<int>(TrainingDataTypes::nParticles);++i)
            item.secFlags.at(i) = item.secFlags.at(i) || secFlags->at(i);
    }

    private:
    long long curEntry, curEntryEvt2, curEntryBg2;
    long long entries;

    int nTrk;
    std::ifstream qdcFile;
    std::string qdcLine;
    TFile *fileEvt, *fileBg;
    TTree *treeEvt1, *treeEvt2, *treeEvt3, *treeBg1, *treeBg2, *treeBg3;

    int parEnum;
    int curEvtId;
    int evtId;

    std::vector<int> *secFlags;
    std::vector<float> *qdc;
    std::vector<float> *tSig;

    std::vector<float> *x; // z
    std::vector<float> *y; // y
    std::vector<TrainingDataTypes::EParticle> *parEnums;
};

#endif
