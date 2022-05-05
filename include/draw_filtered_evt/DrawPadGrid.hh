#ifndef DrawPadGrid_h
#define DrawPadGrid_h 1

#include "TLine.h"
#include "TAttLine.h"

#include <memory>

class DrawPadGrid {
    public:
    DrawPadGrid(const int nPadX, const int nPadY, const double padPlaneX, const double padPlaneY)
        :nPadX{nPadX}, nPadY{nPadY}, padPlaneX{padPlaneX}, padPlaneY{padPlaneY}, grid(new TLine)
    {
        grid->SetLineWidth(1);
        grid->SetNDC(false);
    }

    void Draw()
    {
        for(int x = 1;x < nPadX;++x)
            grid->DrawLine(padPlaneX*x/nPadX, 0., padPlaneX*x/nPadX, padPlaneY);
        for(int y = 1;y < nPadY;++y)
            grid->DrawLine(0., padPlaneY*y/nPadY, padPlaneX, padPlaneY*y/nPadY);
    }

    private:
    const int nPadX, nPadY;
    const double padPlaneX, padPlaneY;
    std::unique_ptr<TLine> grid;
};

#endif

