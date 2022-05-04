#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TTreeReader.h"
#include "TCanvas.h"
#include "TStyle.h"

#include <algorithm> 

// To draw Bragg's peak from output data of macro file ionranges.mac
using namespace std;

std::vector<float> Accumulate(const std::vector<float> &v)
{
    std::vector<float> out;
    float sum = 0;
    out.reserve(v.size());
    for(auto n : v)
    {
        sum += n;
        out.push_back(sum);
    }
    return out;
}

void FillData(const int nStep, const std::vector<float> &stepLen, const std::vector<float> &eDep,
    std::vector<float> &trkLen, std::vector<float> &dEdX)
{
    trkLen = Accumulate(stepLen);
    dEdX.resize(nStep);
    for(size_t i = 0;i < nStep;++i)
    {
        dEdX.at(i) = 10*eDep.at(i)/stepLen.at(i);
    }
}

int DrawBraggPeak(const char *fileName)
{
    auto fileRoot = new TFile(fileName, "READ");

    TTreeReader reader("tree_gc2", fileRoot);
    TTreeReaderValue <int> evtId_(reader, "evtId");
    TTreeReaderValue <int> Nstep_(reader, "Nstep");
    TTreeReaderValue <vector<float>> eDep_(reader, "eDep");
    TTreeReaderValue <vector<float>> stepLen_(reader, "stepLen");
    TTreeReaderValue <vector<float>> pz_(reader, "pz");
    TTreeReaderValue <vector<float>> z_(reader, "z");

    int Nstep;
    std::vector<float> eDep, stepLen, z, pz;
    if(reader.GetEntries() == 0)
    {
        std::cerr << "Error : The tree is empty." << std::endl;
        return -1;
    }

    reader.Next();
    Nstep = *Nstep_;
    eDep = *eDep_;
    stepLen = *stepLen_;
    pz = *pz_;
    z = *z_;
    std::vector<float> trkLen = Accumulate(stepLen);
    std::vector<float> dEdX(stepLen.size());
    std::vector<float> ranges(reader.GetEntries());
    
    FillData(Nstep, stepLen, eDep, trkLen, dEdX);

    float trkLenMax = ceil(*std::max_element(trkLen.begin(), trkLen.end())/10.)*15.;
    float dEdXmax = ceil(*std::max_element(dEdX.begin(), dEdX.end())*1.5);
    float zmin = z.front() - stepLen.front();
    float pzmax = pz.front();
    TH2D *hist1 = new TH2D("hist1", "hist1", 100, 0, trkLenMax, 100, 0, dEdXmax);
    hist1->SetTitle("Bragg Curve of 5 MeV {}^{16}_{ 8}O^{8+} in He(90)/Ar(10) 0.1 atm;Path Length[mm];d#it{E}/d#it{x}[MeV/#it{cm}]");

    TH2D *hist2 = new TH2D("hist2", "hist2", 100, 10, 1.2, 100, 0, 30.);
    hist2->SetTitle("Momentum vs Stopping Power;Momentum [MeV/#it{c}];#it{dE}/#it{dx}[MeV/#it{cm}]");

    TH1D *hist3 = new TH1D("hist3", "hist3", 100, 3*trkLenMax/4, trkLenMax);
    hist3->SetTitle("Projected Ranges of 5 MeV {}^{16}_{ 8}O^{8+} in He(90)/Ar(10) 0.1 atm;Projected Range[mm];cnt");

    reader.Restart();
    while(reader.Next())
    {
        Nstep = *Nstep_;
        eDep = *eDep_;
        stepLen = *stepLen_;
        pz = *pz_;
        z = *z_;
        FillData(Nstep, stepLen, eDep, trkLen, dEdX);
        for(size_t i = 0;i < Nstep;++i)
        {
            hist1->Fill(trkLen.at(i), dEdX.at(i));
            hist2->Fill(pz.at(i), dEdX.at(i));
        }
        hist3->Fill(z.back() - zmin);
    }

    auto c1 = new TCanvas("c1", "c1", 900, 900);
    c1->SetLeftMargin(0.125);
    hist1->Draw("zcol");
    auto c2 = new TCanvas("c2", "c2", 900, 900);
    c2->SetLeftMargin(0.125);
    c2->SetLogx();
    c2->SetLogy();
    hist2->Draw("zcol");

    auto c3 = new TCanvas("c3", "c3", 900, 900);
    c3->SetLeftMargin(0.125);
    hist3->Draw();

    return 0;
}
