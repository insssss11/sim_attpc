#ifndef DrawPadEvent_h
#define DrawPadEvent_h 1

#include "training_data/TrainingDataTypes.hh"
#include "draw_filtered_evt/PadEvent.hh"
#include "config/ParamContainerTable.hh"

#include "TMultiGraph.h"
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
    void InitHist(const PadEvent &padEvent);
    void InitGraphs(const PadEvent &padEvent);

    private:
    EColor GetTrackColor(const TrainingDataTypes::EParticle par);
    std::unique_ptr<TCanvas> canvasHist;
    std::unique_ptr<TCanvas> canvasTrk;
    std::unique_ptr<TH2D> qdcHist;
    std::unique_ptr<TMultiGraph> graphs;
    ParamContainerTable *paramTable;

    // pad plane dimension
    int nPadX, nPadY;
    double padPlaneX, padPlaneY;
    double planeCenterX, planeCenterY;
};

#endif

