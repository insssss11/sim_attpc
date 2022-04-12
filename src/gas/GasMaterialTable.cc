#include "gas/GasMaterialTable.hh"

#include "G4SystemOfUnits.hh"

#include <stdexcept>

using namespace std;

GasMaterialTable *GasMaterialTable::Instance()
{
    static GasMaterialTable *instance = new GasMaterialTable();
    return instance;
}

GasMaterialTable::GasMaterialTable()
{
    Initialize();
}

G4double GasMaterialTable::MolecularWeight(const string &molecule)
{
    if(molecularWeights.find(molecule) == molecularWeights.end())
        throw out_of_range(molecule + " is not registered in table.");
    return molecularWeights[molecule];
}

void GasMaterialTable::Initialize()
{
    InitializeGasMaterials();
    InitializeMolecularWeights();
}

void GasMaterialTable::InitializeGasMaterials()
{
    auto nist = G4NistManager::Instance();
    // Vacuum "Hydrogen gas with very low density"
    G4double density, temp = 293.15*kelvin, pres = 1.0*atmosphere;
    nist->BuildMaterialWithNewDensity("Vacuum", "G4_H", density = 1.e-25*g/cm3, temp, pres);
    nist->BuildMaterialWithNewDensity("He", "G4_He", density = 0.1761*kg/m3, temp, pres);
    nist->BuildMaterialWithNewDensity("Ne", "G4_Ne", density = 8.9990*kg/m3, temp, pres);
    nist->BuildMaterialWithNewDensity("Ar", "G4_Ar", density = 1.7572*kg/m3, temp, pres);
    nist->BuildMaterialWithNewDensity("N2", "G4_N", density = 1.2506*kg/m3, temp, pres);
    nist->BuildMaterialWithNewDensity("CO2", "G4_CARBON_DIOXIDE", density = 1.9359*kg/m3, temp, pres);
    nist->BuildMaterialWithNewDensity("CH4", "G4_METHANE", density = 0.7057*kg/m3, temp, pres);
    nist->BuildMaterialWithNewDensity("iC4H10", "G4_BUTANE", density = 2.5567*kg/m3, temp, pres);
}

void GasMaterialTable::InitializeMolecularWeights()
{
    molecularWeights = {
        {"He", 4.002602}, {"Ne", 20.179}, {"Ar", 39.948}, {"N2", 28.0134}, {"CO2", 44.01},
        {"CH4", 16.043}, {"iC4H10", 58.124}
    };
}


G4Material *GasMaterialTable::FindGasMaterial(const string &gasName)
{
    if(!GasMaterialExists(gasName))
        throw out_of_range("Cannot find G4Material named " + gasName);
    return G4Material::GetMaterial(gasName, false);
}

G4bool GasMaterialTable::GasMaterialExists(const std::string &gasName)
{
    return G4Material::GetMaterial(gasName, false) != nullptr;
}
