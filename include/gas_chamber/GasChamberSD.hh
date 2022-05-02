/// \file GasChamberSD.hh
/// \brief Definition of the GasChamberSD class

#ifndef GasChamberSD_h
#define GasChamberSD_h 1

#include "gas/GasMixtureProperties.hh"
#include "gas_chamber/GasChamberHit.hh"
#include "gas_chamber/GasChamberDigitizer.hh"

#include "tuple/TupleVectorManager.hh"

#include "AnalysisManager.hh"

#include "G4Step.hh"
#include "G4DigiManager.hh"
#include "G4VSensitiveDetector.hh"
#include "G4GenericMessenger.hh"

class GasChamberSD : public G4VSensitiveDetector
{
    public:
    GasChamberSD(G4String name, GasMixtureProperties *gasMixture, G4int verbose = 0);
    virtual ~GasChamberSD();

    virtual void Initialize(G4HCofThisEvent *HCE);
    virtual void EndOfEvent(G4HCofThisEvent *HCE);
    virtual G4bool ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist);

    private:
    void InitManagers();
    void InitDigitizer();
    void FillHitTuples();
    void FillDigiTuples();
    void PrintEndOfEvents();
    
    // for custom command
    void ActivateHitTuples(G4bool Activate = true);
    void ActivateDigiTuples(G4bool Activate = true);
    void DefineCommands();

    private:
    GasMixtureProperties *gasMixtureProperties;
    GasChamberHitsCollection *fHitsCollection;
    GasChamberDigitizer *digitizer;
    G4GenericMessenger *fMessenger;
    
    TupleVectorManager *tupleVectorManager;
    G4AnalysisManager *analysisManager;
    G4DigiManager *digitizerManager;
    
    G4int fHCID;
    G4int fEventId;
    G4int fTrackId;
    G4int fNbOfStepPoints;
    
    G4bool flag;
    G4double theta, Ek, pxv, pyv, pzv, xv, yv, zv, trkLen;
    G4double padPlaneX, padPlaneY, padPlaneZ;
    G4double padCenterX, padCenterY, padCenterZ;

    G4bool hitTupleActivated;
    G4bool digiTupleActivated;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
