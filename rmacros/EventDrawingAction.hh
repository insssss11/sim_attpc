#ifndef EventDrawingAction_h
#define EventDrawingAction_h 1

#include <string>
#include <iostream>

#include "TFile.h"
#include "TH2D.h"
#include "TTreeReader.h"
#include "TCanvas.h"
#include "TApplication.h"
#include "TStyle.h"

// This class is a base class drawing event by event reading array data from tree.
// Call Connect() method to draw the next event every time when signal is emitted.

class EventDrawingAction : public TQObject
{
    public:
    EventDrawingAction(const std::string &clsName, TTree *_tree)
        : TQObject(), className(clsName),
        tree(_tree),
        curEvtNum(0), totalEvtNum(_tree->GetEntries())
    {
    }

    virtual ~EventDrawingAction() noexcept
    {
    }

    void DrawNextEvent()
    {
        if(CheckEndOfEntry())
        {
            std::cout << "Reached the end of entries." << std::endl;
            return;
        }
        ReadNextEvent();
        DrawEvent();
        PrintStatus();
    }

    void Connect(TQObject *qObject, const char *signal)
    {
        qObject->Connect(signal, className.c_str(), this, "DrawNextEvent()");
    }
    
    // pure virtual method : draw anything using information in data
    // Draw anything using information in data
    virtual void DrawEvent() = 0;

    protected:
    TTree *tree;
    
    private:

    bool CheckEndOfEntry()
    {
        return curEvtNum == totalEvtNum;
    }

    void ReadNextEvent()
    {
        tree->GetEntry(curEvtNum);
        ++curEvtNum;
    }

    void PrintStatus()
    {
        std::cout << "Drawing event " << curEvtNum << "/" << totalEvtNum << std::endl;
    }


    private:
    const std::string className;
    Long64_t curEvtNum, totalEvtNum;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

