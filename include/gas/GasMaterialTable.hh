/// \file GasMaterialTable.hh
/// \brief Definition of the GasMaterialTable class

#ifndef GasMaterialTable_h
#define GasMaterialTable_h 1

#include <unordered_map>
#include <string>
#include "G4NistManager.hh"

/// Initialize G4Material instances as components of gas mixtures.
class GasMaterialTable
{
    public:
    static GasMaterialTable *Instance();
    G4double MolecularWeight(const std::string &molecule);
    G4Material *FindGasMaterial(const std::string &gasName);
    G4bool GasMaterialExists(const std::string &gasName);
    
    protected:
    GasMaterialTable();
    
    private:
    void Initialize();
    void InitializeGasMaterials();
    void InitializeMolecularWeights();
    std::unordered_map<std::string, G4double> molecularWeights;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
