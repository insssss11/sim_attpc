#include "draw_filtered_evt/DrawPadEvent.hh"

#include "TGraph.h"

using namespace std;
using namespace TrainingDataTypes;

DrawPadEvent::DrawPadEvent(ParamContainerTable *paramTable) : canvasHist(nullptr), canvasTrk(nullptr), paramTable{paramTable}
{
    InitPadDimensions();
    qdcHist = make_unique<TH2D>("", "", nPadX, 0, padPlaneX, nPadY, 0, padPlaneY);
    qdcHist->SetTitle("Mulplicated ionized charge on pads;#it{x} (mm);#it{y} (mm);#it{charge} (pC)");
    qdcHist->GetXaxis()->SetTitleOffset(1.0);
    qdcHist->GetYaxis()->SetTitleOffset(1.2);
    qdcHist->GetZaxis()->SetTitleOffset(1.2);
    qdcHist->SetStats(false);
}

void DrawPadEvent::InitPadDimensions()
{
    auto container = paramTable->GetContainer("gas_chamber");
    nPadX = container->GetParamI("nPadX");
    nPadY = container->GetParamI("nPadY");
    padPlaneX = container->GetParamD("chamberX");
    padPlaneY = container->GetParamD("chamberY");
    planeCenterX = container->GetParamD("chamberPosX");
    planeCenterY = container->GetParamD("chamberPosY");
}

void DrawPadEvent::Draw(const PadEvent &padEvent)
{
    if(canvasHist == nullptr)
        InitCanvas();
    InitHist(padEvent);
    InitGraphs(padEvent);
    canvasHist->cd();
    qdcHist->Draw("colz");
    canvasTrk->cd();
    graphs->Draw("ALSAME");
    canvasHist->Print("filtered_qdc.png", "png");
    canvasTrk->Print("filtered_trk.png", "png");
}

void DrawPadEvent::InitCanvas()
{
    canvasHist = make_unique<TCanvas>("c1", "c1", 900, 1000);
    canvasHist->SetRealAspectRatio();
    canvasHist->SetLeftMargin(0.1);
    canvasHist->SetRightMargin(0.15);
    canvasTrk = make_unique<TCanvas>("c2", "c2", 900, 1000);
    canvasTrk->SetRealAspectRatio();
    canvasTrk->SetLeftMargin(0.15);
    canvasTrk->SetRightMargin(0.15);
}

void DrawPadEvent::InitHist(const PadEvent &padEvent)
{
    qdcHist->Clear();
    for(int y = 0;y < nPadY;++y)
        for(int x = 0;x < nPadX;++x)
            qdcHist->SetBinContent(x + 1, y + 1, 1e12*padEvent.qdc.at(x + y*nPadX));
}

void DrawPadEvent::InitGraphs(const PadEvent &padEvent)
{
    graphs = make_unique<TMultiGraph>();
    graphs->SetTitle("Tracks on pads;#it{x} (mm);#it{y} (mm);");
    graphs->GetXaxis()->SetLimits(0, padPlaneX);
    graphs->GetYaxis()->SetRangeUser(0, padPlaneY);
    graphs->GetXaxis()->SetTitleOffset(1.0);
    graphs->GetYaxis()->SetTitleOffset(1.0);
    for(size_t i = 0;i < padEvent.parEnums.size();++i)
    {
        try {
            int nP = padEvent.x.at(i).size();
            auto gr = new TGraph(nP);
            gr->SetLineColor(GetTrackColor(padEvent.parEnums.at(i)));
            for(int j = 0;j < nP;++j)
                gr->SetPoint(j,
                padEvent.x.at(i).at(j) + planeCenterX + padPlaneX/2,
                padEvent.y.at(i).at(j) + planeCenterY + padPlaneY/2);
            gr->SetLineWidth(4);
            graphs->Add(gr);
        }
        catch(std::exception const &e)
        {
            cerr << e.what() << endl;
        }
    }
}

EColor DrawPadEvent::GetTrackColor(EParticle par)
{
    switch(par)
    {
        case Carbon:
            return kBlue;
        case Gamma:
            return kGreen;
        case Oxygen:
            return kRed;
        case Alpha:
            return kMagenta;
        case Proton:
            return kCyan;
        default:
            return kWhite;
    }
}