#include "TFile.h"
#include "TTree.h"
#include "TH2D.h"
#include "TTreeReader.h"
#include "TCanvas.h"
#include "TStyle.h"
// To draw Bragg's peak from output data of macro file ionranges.mac
using namespace std;

int DrawPadCharges(const char *fileName, int nPadX, int nPadY, double padPlaneX, double padPlaneY, int evtId = 0)
{
    auto fileRoot = new TFile(fileName, "READ");

    TTreeReader reader("tree_gc3", fileRoot);
    TTreeReaderValue <vector<double> > qdcReader(reader, "qdc");
    int evtId_ = 0;
    int Nstep;
    vector<double> qdc;

    while(reader.Next())
    {
        if(evtId_ == evtId)
        {
            qdc = *qdcReader;
        }
        ++evtId_;
    }

    if(qdc.size() != nPadX*nPadY)
    {
        cout << "Invalid argument error, The total number of pads(= Nxpad X Nypad) is not consistent to the tree structure." << endl;
        return -1;
    }
    double padX = padPlaneX/nPadX, padY = padPlaneY/nPadY;
    int idx = 0;
    auto hist = new TH2D("hist", "", nPadX, 0., padPlaneX, nPadY, 0., padPlaneY);
    for(int y = 0;y < nPadY;++y)
        for(int x = 0;x < nPadX;++x)
        {
            hist->SetBinContent(x, y, qdc.at(x + y*nPadX));
        }

    auto c1 = new TCanvas("c1", "c1", 900, 900);
    c1->SetLeftMargin(0.125);
    hist->GetXaxis()->SetRangeUser(0., padX);
    hist->GetYaxis()->SetRangeUser(0., padY);
    hist->SetTitle("Projection on the pads of mulplicated ionized charge;#it{x} (mm);#it{y} (mm);/#it{charge} (fC)");
    hist->Draw("LEGO");

    return 0;
}