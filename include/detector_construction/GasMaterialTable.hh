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
    static void Initialize();
    static G4double MolecularWeight(const std::string &molecule);
    private:
    static std::unordered_map<std::string, G4double> molecularWeights;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
