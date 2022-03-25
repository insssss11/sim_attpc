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
class GasMixtureProperties
{
    public:
    GasMixtureProperties();
    GasMixtureProperties(const std::vector<std::string> &comps, const std::vector<G4double> &frac,
        const G4double temperature = 273.15*kelvin, const G4double pressure = 1.*atmosphere);

    void SetGasMixture(const std::vector<std::string> &comps, const std::vector<G4double> &fracs);
    void SetTemperature(G4double t);
    void SetPressure(G4double p);
    void SetElectricField(const G4ThreeVector &eField);
    void SetElectricField(const G4double Ex, const G4double Ey, const G4double Ez);
    void SetMagneticField(const G4ThreeVector &bField);
    void SetMagneticField(const G4double Bx, const G4double By, const G4double Bz);

    G4double GetTemprature() const;
    G4double GetPressure() const;
    G4double GetDensity() const;
    G4double GetMeanAtomicWeight() const;
    // in cm/us
    G4ThreeVector GetDriftVelocity() const;
    // in cm^(1/2)
    G4double GetTransverseDiffusion() const;
    G4double GetLongitudinalDiffusion() const;
    
    G4double GetFanoFactor() const;
    G4double GetMeanExcitationEnergy() const;
    G4double GetMeanEnergyPerIonPair() const;
    const std::string GetGasMixtureName() const;
    const std::vector<std::string> GetComponents() const;
    const std::vector<G4double> GetFractions() const;
    private:
    G4bool CheckMagboltzInitialized() const;
    void InitMagboltz();
    const std::string MakeGasMixtureName() const;
    
    private:
    std::vector<std::string> components;
    std::vector<G4double> fractions;
    size_t nComponents;

    // follow unit definition in Geant4
    G4double temperature, pressure;

    G4ThreeVector eField, bField;

    std::string gasMixtureName;
    std::unique_ptr<Garfield::MediumMagboltz> gasMixture;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
