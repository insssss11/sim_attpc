#ifndef GasChamberDigitizer_h
#define GasChamberDigitizer_h 1

#include "gas_chamber/GasChamberDigi.hh"
#include "gas_chamber/GasChamberHit.hh"

#include "G4VDigitizerModule.hh"
#include "G4Step.hh"
#include "globals.hh"

#include <vector>

class GasChamberDigitizerMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// digitizer module for gaschamber readout pads
class GasChamberDigitizer : public G4VDigitizerModule
{
    public:
    GasChamberDigitizer(G4String name,
        G4double _padPlaneX, G4double _padPlaneY, G4double _nPadX, G4double _nPadY,
        G4double eIonPair,
        const G4ThreeVector &_centerPos = G4ThreeVector(0., 0., 0.));
    private:
    void InitPads();
    
    public:
    ~GasChamberDigitizer();

    void Digitize();
    
    void FillPadsInfo(const GasChamberHitsCollection* hitscollection);
    private:
    void FillPadsTrack(const GasChamberHit *hit);
    void FillPadsStep(const G4ThreeVector &ePos, G4double eDep);

    public:
    void SetChargeGain(G4double gain);
    void SetEnergyPerElectronPair(G4double eIonPiar);
    void SetThreshold(G4double threshold);
    void SetPadPlaneCenter(const G4ThreeVector &pos);
    void SetPadMargin(G4double margin);

    void ProjectIonizedElectron(const G4ThreeVector &ePos, int nElectron);

    private:
    const G4double padPlaneX, padPlaneY;
    const G4int nPadX, nPadY;
    G4double padMargin;
    const G4double energyPerElectronPair;
    G4ThreeVector centerPos;
    std::vector<std::vector<GasChamberDigi>> *readoutPads;

    G4double adcThreshold;
    G4double chargeGain;

    GasChamberDigitsCollection*  digitsCollection;
    GasChamberDigitizerMessenger* digiMessenger;
};

#endif








