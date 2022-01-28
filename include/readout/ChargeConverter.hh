/// \file ChargeConverter.hh
/// \brief Definition of the ChargeConverter class

#ifndef ChargeConverter_h
#define ChargeConverter_h 1

#include "readout/Converter.hh"

/// This derived class emulates a QDC module(default charge unit fC).
class ChargeConverter : public Converter
{
    public:
    ChargeConverter(const G4String convName,
        unsigned int nChannel, unsigned int bits, G4double fullScaleInfC);
    virtual ~ChargeConverter();
    virtual G4int Convert(G4double value) const override;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#endif