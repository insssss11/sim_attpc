#ifndef GasChamberDigitizer_h
#define GasChamberDigitizer_h 1

#include "gas/GasMixtureProperties.hh"
#include "gas_chamber/GasChamberDigi.hh"
#include "gas_chamber/GasChamberHit.hh"
#include "gas_chamber/diffusion/DiffusionGaussian.hh"

#include "G4VDigitizerModule.hh"
#include "G4Step.hh"
#include "globals.hh"

#include "Math/IntegratorMultiDim.h"

#include <vector>

class GasChamberDigitizerMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// digitizer module for gaschamber readout pads
class GasChamberDigitizer : public G4VDigitizerModule
{
    public:
    GasChamberDigitizer(G4String name,
        G4double _padPlaneX, G4double _padPlaneY, G4double chamberH, G4int _nPadX, G4int _nPadY,
        const G4ThreeVector &_centerPos = G4ThreeVector(0., 0., 0.), G4double margin = 0.);
    ~GasChamberDigitizer();    

    void SetGasMixtureProperties(GasMixtureProperties *GasMixtureProperties);
    void SetChargeMultiplication(G4double gainMean, G4double gainStd);
    void SetFullScaleRange(G4double fsr);
    void SetThreshold(G4double threshold);
    void SetPadPlaneCenter(const G4ThreeVector &pos);
    void SetPadMargin(G4double margin);  

    void FillChargeOnPads(std::vector<G4float> &vector) const;
    void FillTimeOnPads(std::vector<G4float> &vector) const;

    void Digitize();
    void ClearPads();
    void FillPadsInfo(const GasChamberHitsCollection* hitscollection);

    private:
    void Init();
    void InitPads();
    void InitDiffusion();

    // Physical quantities with fluctuation
    G4double DiffusedDriftLen(G4double driftLen) const;
    G4double MultiplicatedCharge(G4double clusterSize) const;
    // check a pad is close enough to center electron cluster
    G4bool CheckWorthIntegrate(const GasChamberDigi* pad, Double_t cluseterStd, Double_t xe, Double_t ye);
    private:
    void FillPadsTrack(const GasChamberHit *hit);
    void FillPadsStep(const G4ThreeVector &ePos, G4double eDep);

    private:
    // for the integration of 2D gaussian on readout x-y plane
    std::unique_ptr<ROOT::Math::IntegratorMultiDim> totalChargeIntegrator;
    std::unique_ptr<DiffusionGaussian> diffusion;

    // for the diffusion along longitudinal direction.
    G4double diffusionL, driftVel;

    GasMixtureProperties *gasMixtureProperties;
    G4double fsr;
    const G4double padPlaneX, padPlaneY, chamberH;
    const G4int nPadX, nPadY;
    G4double padMargin;
    G4ThreeVector centerPos;
    std::vector<std::vector<GasChamberDigi>> *readoutPads;

    G4double adcThreshold;
    G4double gainMean;
    G4double gainStd;

    GasChamberDigitsCollection*  digitsCollection;
    GasChamberDigitizerMessenger* digiMessenger;

};

#endif








