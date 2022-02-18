#include "EventDrawingAction.hh"

#include "TFile.h"
#include "TH2D.h"
#include "TTreeReader.h"
#include "TCanvas.h"
#include "TApplication.h"
#include "TStyle.h"
// To draw Bragg's peak from output data of macro file ionranges.mac

typedef struct {
    int nPadX;
    int nPadY;
    double padPlaneX;
    double padPlaneY;
} PadPlane;

class DrawPadChargesAction : public EventDrawingAction {
    public:
    DrawPadChargesAction(TTree *tree, const PadPlane &_padPlane)
        : EventDrawingAction("DrawPadChargesAction", tree),
        padPlane(_padPlane)
    {
        data = new std::vector<double>(padPlane.nPadX*padPlane.nPadY);
        tree->SetBranchAddress("qdc", &data);

        hist = new TH2D("hist", "", padPlane.nPadX, 0., padPlane.padPlaneX, padPlane.nPadY, 0., padPlane.padPlaneY);
        hist->SetTitle("Projection on the pads of mulplicated ionized charge;#it{x} (mm);#it{y} (mm);/#it{charge} (fC)");
    }

    virtual ~DrawPadChargesAction()
    {
        delete data;
        delete hist;
    }

    virtual void DrawEvent() override
    {
        for(int y = 0;y < padPlane.nPadY;++y)
            for(int x = 0;x < padPlane.nPadX;++x)
                hist->SetBinContent(x, y, data->at(x + y*padPlane.nPadX));
        hist->Draw("LEGO");
    }
    
    private:
    TH2D *hist;
    const PadPlane padPlane;
    std::vector<double> *data;
};

int DrawPadCharges(const char *fileName, int nPadX, int nPadY, double padPlaneX, double padPlaneY)
{
    auto c1 = new TCanvas("c1", "c1", 900, 900);
    c1->SetLeftMargin(0.125);
    auto fileRoot = new TFile(fileName, "READ");
    auto tree = (TTree*)fileRoot->Get("tree_gc3");

    DrawPadChargesAction *action = new DrawPadChargesAction(tree, {nPadX, nPadY, padPlaneX, padPlaneY});
    action->Connect(gApplication, "ReturnPressed(const char*)");
    // action->DisconnectWhenClosed(c1);
    return 0;
}