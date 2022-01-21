#include "TFile.h"
#include "TTree.h"
#include "TGraph.h"
#include "TTreeReader.h"
#include "TCanvas.h"
#include "TStyle.h"
// To draw Bragg's peak from output data of macro file ionranges.mac
using namespace std;

int DrawBraggsCurve(const char *fileName, int evtId = 0)
{
    auto fileRoot = new TFile(fileName, "READ");

    TTreeReader reader("tree_gc2", fileRoot);
    TTreeReaderValue <int> evtId_(reader, "evtId");
    TTreeReaderValue <int> Nstep_(reader, "Nstep");
    TTreeReaderValue <vector<double>> eDep_(reader, "eDep");
    TTreeReaderValue <vector<double>> stepLen_(reader, "stepLen");

    int Nstep;
    vector<double> eDep;
    vector<double> stepLen;

    while(reader.Next())
    {
        if(*evtId_ == evtId)
        {
            eDep = *eDep_;
            stepLen = *stepLen_;
            Nstep = *Nstep_;
            break;
        }
    }

    auto gr = new TGraph(Nstep);
    double trkLen = 0;
    for(int i = 0; i < eDep.size(); i++)
    {
        trkLen += stepLen[i];
        gr->SetPoint(i, trkLen, 10*eDep[i]/stepLen[i]);
    }
    auto c1 = new TCanvas("c1", "c1", 900, 900);
    c1->SetLeftMargin(0.125);
    gr->SetTitle("Bragg's Curve of 4 MeV {}^{12}_{ 6}C^{4+} in He(90)/Ar(10);trkLen (mm);d#it{E}/d#it{x} (MeV/#it{cm})");
    gr->SetMarkerStyle(kFullDotMedium);
    gr->Draw("AP");

    return 0;
}