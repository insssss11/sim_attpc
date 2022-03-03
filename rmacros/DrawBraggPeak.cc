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

std::vector<double> Accumulate(const std::vector<double> &v)
{
    std::vector<double> out;
    double sum = 0;
    out.reserve(v.size());
    for(auto n : v)
    {
        sum += n;
        out.push_back(sum);
    }
    return out;
}

void FillData(const int nStep, const std::vector<double> &stepLen, const std::vector<double> &eDep,
    std::vector<double> &trkLen, std::vector<double> &dEdX)
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
    TTreeReaderValue <vector<double>> eDep_(reader, "eDep");
    TTreeReaderValue <vector<double>> stepLen_(reader, "stepLen");
    TTreeReaderValue <vector<double>> pz_(reader, "pz");
    TTreeReaderValue <vector<double>> z_(reader, "z");

    int Nstep;
    std::vector<double> eDep, stepLen, z, pz;
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
    std::vector<double> trkLen = Accumulate(stepLen);
    std::vector<double> dEdX(stepLen.size());
    std::vector<double> ranges(reader.GetEntries());
    
    FillData(Nstep, stepLen, eDep, trkLen, dEdX);

    double trkLenMax = *std::max_element(trkLen.begin(), trkLen.end());
    double dEdXmax = *std::max_element(dEdX.begin(), dEdX.end());
    double zmin = z.front() - stepLen.front();
    double pzmax = pz.front();
    TH2D *hist1 = new TH2D("hist1", "hist1", 100, 0, 1.5*trkLenMax, 100, 0, 1.5*dEdXmax);
    hist1->SetTitle("Bragg Curve of 4 MeV {}^{12}_{ 6}C^{4+} in He(90)/Ar(10);trkLen (mm);d#it{E}/d#it{x} (MeV/#it{cm})");

    TH2D *hist2 = new TH2D("hist2", "hist2", 100, 10, 1.2*pzmax, 100, 0, 1.5*dEdXmax);
    hist2->SetTitle("Momentum vs Stopping Power;Momentum (MeV/c);d#it{E}/d#it{x} (MeV/#it{cm})");

    TH1D *hist3 = new TH1D("hist3", "hist3", 100, 0.8*trkLenMax, 1.2*trkLenMax);
    hist3->SetTitle("Projected Ranges of Ion;Projected Range(mm);cnt");

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