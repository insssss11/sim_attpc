#ifndef DrawPadEvent_h
#define DrawPadEvent_h 1

// #include "training_data/TrainingDataTypes.hh"
#include "draw_filtered_evt/PadEvent.hh"
#include "config/ParamContainerTable.hh"
#include "draw_filtered_evt/DrawPadGrid.hh"

#include "TPolyMarker.h"
#include "TLine.h"
#include "TAttLine.h"
#include "TH2D.h"
#include "TCanvas.h"
#include "TString.h"

#include <memory>
#include <vector>


class DrawPadEvent{
    public:
    DrawPadEvent(ParamContainerTable *paramTable);
    ~DrawPadEvent();
    void Draw(const PadEvent &padEvent, const TString &name);
    
    private:
    void CheckCanvas();

    void InitCanvas();
    void InitPadDimensions();
    void DrawCharges(const PadEvent &padEvent);
    void DrawTracks(const PadEvent &padEvent);
    void DrawGrid();
    void SetDirectory(const std::string &dir);
    private:
    EColor GetTrackColor(const TrainingDataTypes::EParticle par);
    std::unique_ptr<TCanvas> canvas;
    std::unique_ptr<TH2D> qdcHist;
    std::unique_ptr<DrawPadGrid> gridDrawer;
    std::vector<TPolyMarker*> tracks;
    ParamContainerTable *paramTable;

    std::string dir;

    static constexpr size_t trackBuffer = 200;
    // pad plane dimension
    int nPadX, nPadY;
    double padPlaneX, padPlaneY;
    double planeCenterX, planeCenterY;
};

#endif

