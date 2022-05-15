#include "draw_filtered_evt/DrawPadEvent.hh"

#include "TGraph.h"

using namespace std;
using namespace TrainingDataTypes;

DrawPadEvent::DrawPadEvent(ParamContainerTable *paramTable)
    : canvas(nullptr), paramTable{paramTable}, dir("./")
{
    InitPadDimensions();
    qdcHist = make_unique<TH2D>("", "Mulplicated ionized charge on pads", nPadX, 0, padPlaneX, nPadY, 0, padPlaneY);
    qdcHist->SetTitle("Mulplicated ionized charge on pads;#it{z} [mm];#it{y} [mm];#charge [pC]");
    qdcHist->GetXaxis()->SetTitleOffset(1.0);
    qdcHist->GetXaxis()->SetTickLength(0.);
    qdcHist->GetYaxis()->SetTitleOffset(1.2);
    qdcHist->GetYaxis()->SetTickLength(0.);
    qdcHist->GetZaxis()->SetTitleOffset(1.3);
    qdcHist->SetStats(false);
    for(size_t i = 0;i < trackBuffer;++i)
        tracks.push_back(new TPolyMarker);
}

DrawPadEvent::~DrawPadEvent()
{
    for(auto &markerPtr : tracks)
        delete markerPtr;
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
    gridDrawer = make_unique<DrawPadGrid>(nPadX, nPadY, padPlaneX, padPlaneY);
}

void DrawPadEvent::Draw(const PadEvent &padEvent, const TString &name)
{
    static int figNum = 0;
    if(canvas == nullptr)
        InitCanvas();
    canvas->cd();
    DrawCharges(padEvent);
    DrawTracks(padEvent);
    if(name.EndsWith(".eps"))
        canvas->Print(name, "eps");
    else if(name.EndsWith(".png"))
        canvas->Print(name, "png");
    else if(name.EndsWith(".svg"))
        canvas->Print(name, "svg");
    else if(name.EndsWith(".jpg"))
        canvas->Print(name, "jpg");
    else if(name.EndsWith(".pdf"))
        canvas->Print(name, "pdf");
    else{
        cout << "Cannot identify file extension to save : " << name << endl;
        return;
    }
    ++figNum;
}

void DrawPadEvent::InitCanvas()
{
    canvas = make_unique<TCanvas>("c1", "c1", 900, 1000);
    canvas->SetRealAspectRatio();
    canvas->SetLeftMargin(0.1);
    canvas->SetRightMargin(0.15);
}

void DrawPadEvent::DrawCharges(const PadEvent &padEvent)
{
    qdcHist->Reset("ICESM");
    for(int y = 0;y < nPadY;++y)
        for(int x = 0;x < nPadX;++x)
            qdcHist->SetBinContent(x + 1, y + 1, 1e12*padEvent.qdc.at(x + y*nPadX));
    qdcHist->Draw("colz");
    gridDrawer->Draw();
}

void DrawPadEvent::DrawTracks(const PadEvent &padEvent)
{
    for(size_t i = 0;i < padEvent.parEnums.size();++i)
    {
        try {
            const int nP = padEvent.x.at(i).size();
            tracks.at(i)->SetPolyMarker(-1);
            tracks.at(i)->SetPolyMarker(nP);
            tracks.at(i)->SetMarkerColor(GetTrackColor(padEvent.parEnums.at(i)));
            tracks.at(i)->SetMarkerStyle(20);
            tracks.at(i)->SetMarkerSize(0.6);
            for(int j = 0;j < nP;++j)
                tracks.at(i)->SetPoint(j,
                padEvent.x.at(i).at(j) + planeCenterX + padPlaneX/2,
                padEvent.y.at(i).at(j) + planeCenterY + padPlaneY/2);
            tracks.at(i)->Draw("APSAME");
        }
        catch(std::exception const &e)
        {
            cerr << "DrawPadEvent::DrawTracks : Track buffer full(maximum size " << trackBuffer << ") : " << e.what() << endl;
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

void DrawPadEvent::SetDirectory(const std::string &dir)
{
    this->dir = dir;
}
