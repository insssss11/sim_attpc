/// \file ChargeConverter.cc
/// \brief Implementation of the ChargeConverter class

#include "readout/ChargeConverter.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ChargeConverter::ChargeConverter(const G4String name_,
    unsigned int nChannel_, unsigned int bits_, G4double fullScaleInfC)
    : Converter(name_, nChannel_, bits_, fullScaleInfC)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ChargeConverter::~ChargeConverter()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int ChargeConverter::Convert(G4double value) const
{
    return (G4int)value*fsr/bits;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
