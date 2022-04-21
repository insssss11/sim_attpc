#include "gas_chamber/diffusion/DiffusionGaussian.hh"

#include "TMath.h"

#include "G4SystemOfUnits.hh"

using namespace TMath;

DiffusionGaussian::DiffusionGaussian()
    : diffusionCoef{0.}, intrinsicStd{0.}, driftVel{0.},
    stdstd{0}, charge{0.}, xpos{0}, ypos{0}
{
}

DiffusionGaussian *DiffusionGaussian::Clone() const
{
    auto diffusion = new DiffusionGaussian;
    diffusion->stdstd = stdstd;
    diffusion->intrinsicStd = intrinsicStd;
    diffusion->driftVel = driftVel;
    diffusion->stdstd = stdstd;
    diffusion->charge = charge;
    diffusion->xpos = ypos;
    diffusion->ypos = ypos;
    return diffusion;
}

unsigned int DiffusionGaussian::NDim() const
{
    return 2;
}

G4double DiffusionGaussian::GetCloudStd() const
{
    return Sqrt(stdstd);
}

G4double DiffusionGaussian::GetCharge() const
{
    return charge;
}

void DiffusionGaussian::SetDiffusionCoef(G4double diff)
{
    diffusionCoef = diff;
}

void DiffusionGaussian::SetIntrinsicDiff(G4double std)
{
    intrinsicStd = std;
}

void DiffusionGaussian::SetCharge(G4double q)
{
    
    charge = q;
}

void DiffusionGaussian::SetPosition(G4double x, G4double y)
{
    xpos = x;
    ypos = y;
}

void DiffusionGaussian::SetDriftLen(G4double l)
{
    // diffusion spread = D_t*sqrt(length), in the unit of D_t : cm^(1/2)
    stdstd = diffusionCoef*diffusionCoef*cm*l + intrinsicStd*intrinsicStd;
}

double DiffusionGaussian::DoEval(const double *x) const
{
    G4double xx = (x[0] - xpos)*(x[0] - xpos);
    G4double yy = (x[1] - ypos)*(x[1] - ypos);
    return charge*Exp(-(xx + yy)/(2*stdstd))/(stdstd*TwoPi());
}