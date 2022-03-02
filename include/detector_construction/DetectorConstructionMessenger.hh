/// \file DetectorConstructionMessenger.hh
/// \brief Declaration of the DetectorConstructionMessenger class

#ifndef DetectorConstructionMessenger_h
#define DetectorConstructionMessenger_h 1

#include "DetectorConstruction.hh"

#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"

#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWith3VectorAndUnit.hh"

class DetectorConstruction;

// a messenger class controlling DetectorConstruction class.
class DetectorConstructionMessenger : public G4UImessenger
{
public:
    DetectorConstructionMessenger(DetectorConstruction *detector);
    virtual ~DetectorConstructionMessenger();

    void SetNewValue(G4UIcommand * command, G4String newValues);
private:
    void PassArgsToSetGas(const G4String &newValues);
private:
    DetectorConstruction *fDetector;
    G4UIdirectory *fDetectorDirectory;
    // UI commands
    G4UIcommand *fSetGasCmd;
    G4UIcmdWithADoubleAndUnit *fSetMaxStep;
    G4UIcmdWithADoubleAndUnit *fSetMaxTrack;
    G4UIcmdWithADoubleAndUnit *fSetMaxTime;
    G4UIcmdWithADoubleAndUnit *fSetMinKinE;
    G4UIcmdWithADoubleAndUnit *fSetBeamPipePosY;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
