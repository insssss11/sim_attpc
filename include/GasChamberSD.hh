/// \file GasChamberSD.hh
/// \brief Definition of the GasChamberSD class

#ifndef GasChamberSD_h
#define GasChamberSD_h 1

#include "G4VSensitiveDetector.hh"

#include "GasChamberHit.hh"
#include "G4GenericMessenger.hh"
class G4Step;
class G4HCofThisEvent;
class G4TouchableHistory;

/// EM calorimeter sensitive detector

class GasChamberSD : public G4VSensitiveDetector
{
    public:
    GasChamberSD(G4String name, G4int verbose = 0);
    virtual ~GasChamberSD();

    virtual void Initialize(G4HCofThisEvent *HCE);
    virtual void EndOfEvent(G4HCofThisEvent *HCE);
    virtual G4bool ProcessHits(G4Step *aStep, G4TouchableHistory *ROhist);
    private:
    GasChamberHitsCollection *fHitsCollection;
    G4GenericMessenger *fMessenger;
    G4int fHCID;
    G4int fEventId;
    G4int fTrackId;
    G4int fSizeOfHCE;
    G4int fNbOfStepPoints;
    static constexpr G4int kNbOfSkipPrint = 10;

    private:
    void DefineCommands();
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
