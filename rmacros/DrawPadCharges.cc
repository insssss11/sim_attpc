#include "TFile.h"
#include "TTree.h"
#include "TH2D.h"
#include "TTreeReader.h"
#include "TCanvas.h"
#include "TStyle.h"
#include "TSystem.h"
// To draw Bragg's peak from output data of macro file ionranges.mac

typedef struct {
    int nPadX;
    int nPadY;
    double padPlaneX;
    double padPlaneY;
} PadPlane;

PadPlane padPlane;
std::vector<double> qdc;
TH2D *hist;
TCanvas *c1;

std::string GetInput()
{
    std::string input;
    std::cout << "Enter(press .q to quit or any key to draw the next event) : ";
    std::getline(std::cin, input);
    return input;
}

bool CheckArgsValidity()
{
    if(qdc.size() != padPlane.nPadX*padPlane.nPadY)
        return false;
    else
        return true;
}

bool DrawNextEvent(TTreeReader &reader, TTreeReaderValue <vector<double> > &qdcReader)
{
    static int evtNum = 0;
    if(!reader.Next())
    {
        std::cout << "Current event number reached the max entry number." << std::endl;
        c1->Close();
        return false;
    }
    else
    {
        qdc = *qdcReader;
        if(!CheckArgsValidity())
        {
            std::cerr << "Error : the size of qdc is not consistent with pad plane dimension(nPadX X nPadY)." << std::endl;
            c1->Close();
            return false;
        }
    }
    ++evtNum;
    std::cout << "Drawing event : " << evtNum << "/" << reader.GetEntries() << std::endl;
    double padX = padPlane.padPlaneX/padPlane.nPadX, padY = padPlane.padPlaneY/padPlane.nPadY;
    for(int y = 0;y < padPlane.nPadY;++y)
        for(int x = 0;x < padPlane.nPadX;++x)
        {
            hist->SetBinContent(x, y, qdc.at(x + y*padPlane.nPadX));
        }
    hist->GetXaxis()->SetRangeUser(0., padPlane.padPlaneX);
    hist->GetYaxis()->SetRangeUser(0., padPlane.padPlaneY);
    hist->Draw("LEGO");
    c1->Modified();
    c1->Update();
    gSystem->ProcessEvents();
    return true;
}

int DrawPadCharges(const char *fileName, int nPadX, int nPadY, double padPlaneX, double padPlaneY)
{
    auto fileRoot = new TFile(fileName, "READ");

    padPlane = {nPadX, nPadY, padPlaneX, padPlaneY};

    TTreeReader reader = TTreeReader("tree_gc3", fileRoot);
    TTreeReaderValue <vector<double> > qdcReader = TTreeReaderValue <vector<double> >(reader, "qdc");

    hist = new TH2D("hist", "", nPadX, 0., padPlaneX, nPadY, 0., padPlaneY);
    hist->SetTitle("Projection on the pads of mulplicated ionized charge;#it{x} (mm);#it{y} (mm);/#it{charge} (fC)");

    c1 = new TCanvas("c1", "c1", 900, 900);
    c1->SetLeftMargin(0.125);
    std::string input;
    do {
        if(GetInput() == ".q")
            break;
    } while(DrawNextEvent(reader, qdcReader));
    delete c1;
    delete hist;
    return 0;
}