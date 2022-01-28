/// \file Converter.hh
/// \brief Definition of the Converter class

#ifndef Converter_h
#define Converter_h 1

#include "globals.hh"
#include "G4String.hh"

/// This abstract class is interface to converter (QDC or TDC ...).
class Converter
{
    public:
    Converter(const G4String name_,
        unsigned int nChannel_, unsigned int bits_, G4double fsr_)
        : nChannel(nChannel_), bits(bits_), fsr(fsr_), name(name_) {}

    virtual ~Converter(){}
    virtual G4int Convert(G4double value) const = 0;

    G4String GetName() const { return name; }
    G4double GetFSR() const { return fsr; }
    unsigned int GetBits() const { return bits; }

    protected:
    const unsigned int nChannel, bits;
    G4double fsr; // full scale range
    
    private:
    G4String name;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#endif