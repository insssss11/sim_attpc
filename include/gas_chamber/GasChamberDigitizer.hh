#ifndef GasChamberDigitizer_h
#define GasChamberDigitizer_h 1

#include "gas/GasMixtureProperties.hh"
#include "gas_chamber/GasChamberDigi.hh"
#include "gas_chamber/GasChamberHit.hh"
#include "gas_chamber/diffusion/DiffusionGaussian.hh"
#include "config/ParamContainerTable.hh"
#include "training_data/TrainingDataTypes.hh"

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
    GasChamberDigitizer(const G4String &name);
    ~GasChamberDigitizer();    

    void SetGasMixtureProperties(GasMixtureProperties *GasMixtureProperties);
    void UpdateGasProperties();

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
    void InitPads(const ParamContainer *container);
    void InitMulplier(const ParamContainer *container);

    // Physical quantities with fluctuation
    G4double DiffusedDriftLen(G4double driftLen) const;
    G4double MultiplicatedCharge(G4double clusterSize) const;
    // check a pad is close enough to center electron cluster
    G4bool CheckWorthIntegrate(const GasChamberDigi* pad, Double_t cluseterStd, Double_t xe, Double_t ye);
    private:
    void FillPadsTrack(const GasChamberHit *hit);
    void FillPadsStep(const G4ThreeVector &ePos, G4double eDep, TrainingDataTypes::EParticle parEnum);

    G4bool CheckFillHist(const GasChamberDigi &pad, const G4ThreeVector &ePos, G4double eDep, TrainingDataTypes::EParticle parEnum) const;
    private:
    // for the integration of 2D gaussian on readout x-y plane
    std::unique_ptr<ROOT::Math::IntegratorMultiDim> totalChargeIntegrator;
    std::unique_ptr<DiffusionGaussian> diffusion;

    // for the diffusion along longitudinal direction.
    G4double diffusionL, driftVel;

    GasMixtureProperties *gasMixtureProperties;
    G4double chargeFSR;
    G4double padPlaneX, padPlaneY, chamberH;
    G4int nPadX, nPadY;
    G4double padMargin;
    G4ThreeVector centerPos;
    std::vector<std::vector<GasChamberDigi>> *readoutPads;

    G4double adcThreshold;

    G4double collEff;
    G4double gainMean;
    G4double gainTheta;
    G4double gainStd;

    GasChamberDigitsCollection*  digitsCollection;
    GasChamberDigitizerMessenger* digiMessenger;
};

#endif








