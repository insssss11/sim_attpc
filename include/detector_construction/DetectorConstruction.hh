/// \file DetectorConstruction.hh
/// \brief Definition of the DetectorConstruction class

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "config/ParamContainer.hh"
#include "detector_construction/DetectorConstructionMessenger.hh"
#include "gas/GasMixtureProperties.hh"
#include "gas/GasMaterialTable.hh"

#include "G4UniformMagField.hh"
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
#include <memory>

class DetectorConstructionMessenger;

/// Detector construction
class DetectorConstruction : public G4VUserDetectorConstruction
{
    public:
    DetectorConstruction();
    virtual ~DetectorConstruction();

    virtual G4VPhysicalVolume* Construct();
    virtual void ConstructSDandField();

    void SetGasMixture(
        const std::vector<std::string> &comps, const std::vector<G4int> &fracs,
        const G4int pressure, const G4double temperature = 273.15*kelvin);

    void SetMagneticField(const G4ThreeVector &bField);
    void SetElectricField(const G4ThreeVector &eFeidl);
    void SetBeamPipePosY(G4double posY);

    GasMixtureProperties *GetGasMixtureProperties() const;
    G4UserLimits *GetUserLimits() const;

    G4bool IsInitialized();
    private:

    void Initialize();
    void InitializeGasMixture();

    void UpdateGasMaterial();
    void UpdateLogicVolumes();
    // overall geometry
    void ConstructGeometry();
    // individuals
    void BuildMagnet();
    void BuildMagField();
    void BuildGas();
    void BuildChamber();
    void BuildBeamPipe();

    void SetVisAttributes();

    private:
    GasMaterialTable *gasMaterialTable;
    const ParamContainer *paramContainer;
    DetectorConstructionMessenger *fMessenger;

    G4UserLimits *fUserLimits;

    static G4ThreadLocal G4UniformMagField *fMagneticField;
    static G4ThreadLocal G4FieldManager *fFieldManager;

    G4RotationMatrix *fGeoRotation;

    // Option to switch on/off checking of volumes overlaps    
    const G4bool fCheckOverlaps;

    G4LogicalVolume *fLogicWorld, *fLogicMagnet, *fLogicMagField, *fLogicGas, *fLogicChamber, *fLogicPipe;
    G4PVPlacement *fPhysWorld, *fPhysMagnet, *fPhysMagField, *fPhysGas, *fPhysChamber, *fPhysPipe;

    std::vector<G4VisAttributes*> fVisAttributes;

    G4Material *fGasMat;
    std::unique_ptr<GasMixtureProperties> gasMixtureProperties;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
