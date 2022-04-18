#ifndef DrawPadEvent_h
#define DrawPadEvent_h 1

#include "training_data/TrainingDataTypes.hh"
#include "draw_filtered_evt/PadEvent.hh"
#include "config/ParamContainerTable.hh"

#include "TPolyMarker.h"
#include "TAttLine.h"
#include "TH2D.h"
#include "TCanvas.h"

#include <memory>
#include <vector>


class DrawPadEvent{
    public:
    DrawPadEvent(ParamContainerTable *paramTable);
    void Draw(const PadEvent &padEvent);
    
    private:
    void CheckCanvas();

    void InitCanvas();
    void InitPadDimensions();
    void DrawCharges(const PadEvent &padEvent);
    void DrawTracks(const PadEvent &padEvent);

    private:
    EColor GetTrackColor(const TrainingDataTypes::EParticle par);
    std::unique_ptr<TCanvas> canvas;
    std::unique_ptr<TH2D> qdcHist;
    std::vector<TPolyMarker*> tracks;
    ParamContainerTable *paramTable;

    static constexpr size_t trackBuffer = 20;
    // pad plane dimension
    int nPadX, nPadY;
    double padPlaneX, padPlaneY;
    double planeCenterX, planeCenterY;
};

#endif

