#ifndef DiffusionGaussian_h
#define DiffusionGaussian_h 1

#include "Math/IFunction.h"
#include "globals.hh"

class DiffusionGaussian : public ROOT::Math::IBaseFunctionMultiDim
{
    public:
    DiffusionGaussian();
    
    virtual DiffusionGaussian *Clone() const override;
    virtual unsigned int NDim() const override;

    G4double GetClusterStd() const;
    G4double GetCharge() const;
    void SetDiffusionCoef(G4double diff);
    void SetIntrinsicDiff(G4double std);
    void SetCharge(G4double q);
    void SetPosition(G4double x, G4double y);
    void SetDriftLen(G4double l);
    private:
    virtual double DoEval(const double *x) const override;

    private:
    G4double diffusionCoef, intrinsicStd, driftVel;
    G4double stdstd, charge, xpos, ypos;
};

#endif