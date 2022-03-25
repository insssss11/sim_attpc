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
#include "G4UIcmdWithoutParameter.hh"

class DetectorConstruction;

// a messenger class controlling DetectorConstruction class.
class DetectorConstructionMessenger : public G4UImessenger
{
public:
    DetectorConstructionMessenger(DetectorConstruction *detector);
    virtual ~DetectorConstructionMessenger();

    void SetNewValue(G4UIcommand * command, G4String newValues);
private:

    void InitGasCommands();
    void InitGeometryCommands();
    void InitFieldCommands();

    void PassArgsToSetGasComposition(const G4String &newValues);
private:
    DetectorConstruction *fDetector;
    
    G4UIdirectory *gasDirectory;
    G4UIcommand *fSetGasCmd;
    G4UIcmdWithoutParameter *printGasStatsCmd;
    G4UIcmdWithADoubleAndUnit *setPressureCmd;
    G4UIcmdWithADoubleAndUnit *fSetMaxStep;
    G4UIcmdWithADoubleAndUnit *fSetMaxTrack;
    G4UIcmdWithADoubleAndUnit *fSetMaxTime;
    G4UIcmdWithADoubleAndUnit *fSetMinKinE;

    G4UIdirectory *geometryCmd;
    G4UIcmdWithADoubleAndUnit *fSetBeamPipePosY;

    G4UIdirectory *fieldDirectory;
    G4UIcmdWith3VectorAndUnit *setMagneticFieldCmd;
    G4UIcmdWith3Vector *setElectricFieldCmd;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
