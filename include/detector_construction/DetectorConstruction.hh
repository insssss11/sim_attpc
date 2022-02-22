/// \file DetectorConstruction.hh
/// \brief Definition of the DetectorConstruction class

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "globals.hh"
#include "config/ParamContainer.hh"
#include "detector_construction/DetectorConstructionMessenger.hh"
#include "MagneticField.hh"

#include "G4VSolid.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalVolume.hh"

#include "G4RotationMatrix.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4VisAttributes.hh"
#include "G4FieldManager.hh"
#include "G4UserLimits.hh"
#include "G4Material.hh"

#include <vector>
#include <string>
#include <unordered_map>

class DetectorConstructionMessenger;

/// Detector construction
class DetectorConstruction : public G4VUserDetectorConstruction
{
    public:
    DetectorConstruction();
    virtual ~DetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
    virtual void ConstructSDandField();

    // messenger commands for gas control
    // Gas mixture density is estimated using the ideal gas law, so it may be unaccurate in a certain situation.    
    void SetGas(const G4String &gas1, G4double frac1, const G4String &gas2, G4double frac2, G4double pressure);
    void SetLimitStep(G4double ustepMax);
    void SetLimitTrack(G4double utrakMax);
    void SetLimitTime(G4double utimeMax);
    void SetMinKinE(G4double uekinMax);

    private:
    void ConstructMaterials();
    // overall geometry
    void ConstructGeometry();
    // individuals
    void BuildMagnet();
    void BuildMagField();
    void BuildGas();
    void BuildChamber();
    void BuildBeamPipe();
    
    void SetVisAttributes();

    void RegisterGasMat(const G4String &key, const G4String &val);
    G4Material *FindGasMat(const G4String &key);
    G4String GetGasMixtureStat();
    private:
    const ParamContainer *paramContainer;
    DetectorConstructionMessenger *fMessenger;

    G4UserLimits *fUserLimits;

    static G4ThreadLocal MagneticField *fMagneticField;
    static G4ThreadLocal G4FieldManager *fFieldManager;
    
    // Option to switch on/off checking of volumes overlaps
    G4RotationMatrix *fGeoRotation;
    const G4bool fCheckOverlaps;
    G4LogicalVolume *fLogicWorld, *fLogicMagnet, *fLogicMagField, *fLogicGas, *fLogicChamber, *fLogicPipe;
    G4PVPlacement *fPhysWorld, *fPhysMagnet, *fPhysMagField, *fPhysGas, *fPhysChamber, *fPhysPipe;

    std::vector<G4VisAttributes*> fVisAttributes;
    // variables containing gas information
    G4Material *fGasMat;
    G4String fGasName1, fGasName2;
    G4double fFrac1, fFrac2;
    G4double fPressure;

    // The starting number of waring that too many G4Material instances
    // by calling gas properties(density, mixture .....)
    static constexpr int kNbOfGatMatWarning = 10;
    int fNbOfGasMat;
    // key : gas name, Value : pair of pointer to material and its density at 1 atm
    std::unordered_map<std::string, std::string> *fGasMatMap;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
