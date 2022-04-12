/// \file GasMixtureProperties.hh
/// \brief Definition of the GasMixtureProperties class

#ifndef GasMixtureProperties_h
#define GasMixtureProperties_h 1

#include "Garfield/MediumMagboltz.hh"

#include "G4Material.hh"
#include "G4ThreeVector.hh"
#include "G4SystemOfUnits.hh"

#include <memory>
#include <vector>
#include <string>

// This class calculates physical properties of the given gas mixture.
// All physical quantities are in the system of units of Geant4 otherwise specified.
class GasMixtureProperties
{
    private:
    // This structure contains physical properties supposed to be updated by MediumMagboltz interface
    struct GasPhysicalProperties {
        G4double density;
        G4double fanoFactor;
        G4double meanExcitationEnergy, meanEnergyPerIonPair;
        G4double driftVel;
        G4double diffTrans, diffLongi;
        G4double attachment;
    };
    public:
    GasMixtureProperties();
    GasMixtureProperties(const std::vector<std::string> &comps, const std::vector<G4int> &fracs,
        const G4int pressure = 760, const G4double temperature = 273.15*kelvin);

    // fractions in percentage.
    void SetGasMixture(const std::vector<std::string> &comps, const std::vector<G4int> &fracs,
        const G4int pressure = 760, const G4double temperature = 273.15*kelvin);
    
    void SetElectricField(const G4ThreeVector &eField);
    void SetElectricField(const G4double Ex, const G4double Ey, const G4double Ez);
    void SetMagneticField(const G4ThreeVector &bField);
    void SetMagneticField(const G4double Bx, const G4double By, const G4double Bz);

    G4Material *GetGasMaterial() const;
    G4double GetFanoFactor() const;
    G4double GetW() const;
    G4double GetI() const;
    // in cm/us
    G4double GetDriftVelocity() const;
    // in cm^(1/2)
    G4double GetTransverseDiffusion() const;
    G4double GetLongitudinalDiffusion() const;
    G4double GetElectronAttachment() const;
    const std::string GetGasMixtureName() const;
    
    void PrintGasFileList();        
    void DumpProperties() const;

    private:
    static void InitGasFileList();
    const std::string MakeGasMixtureName(const std::vector<std::string> &comps, const std::vector<G4int> &fracs,
        const G4int pressure, const G4double temperature) const;
    const std::string MakeGasFileName(const std::vector<std::string> &comps, const std::vector<G4int> &fracs,
        const G4int pressure) const;

    // update all parameters and ptr to magboltz and G4Material
    void Update();
    void UpdateMagboltz();
    void UpdateMaterial();
    void UpdateParameters();
    
    void CheckArguments(const std::vector<std::string> &comps, const std::vector<G4int> &fracs,
        const G4int pressure, const G4double temperature);
    G4bool CheckFractionSum(const std::vector<G4int> &fracs) const;
    G4bool IsGasFileApplicable(const std::vector<std::string> &comps, const std::vector<G4int> &fracs, const G4int pressure) const;
    G4bool IsMagboltzApplicable(const std::vector<std::string> &comps) const;
    G4bool IsGeantApplicable(const std::vector<std::string> &comps) const;
    private:
    // Update() is called when variables below get modified.
    std::vector<std::string> components;
    std::vector<G4int> fractions; // in percentage
    size_t nComponents;
    G4ThreeVector eField, bField; // V/cm
    G4int pressure; // in torr
    G4double temperature;
    std::string gasMixtureName;
    std::string gasFileName;
    GasPhysicalProperties gasPhysicalProperties;
    std::unique_ptr<Garfield::MediumMagboltz> gasMagboltz;
    G4Material *gasMaterial;

    static std::string gasFileDir;
    static std::unordered_map<std::string, G4int> gasFileList;
    static std::unordered_map<std::string, G4int> magboltzGasList;
};
#endif
