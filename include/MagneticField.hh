/// \file MagneticField.hh
/// \brief Definition of the MagneticField class

#ifndef MagneticField_H
#define MagneticField_H 1

#include "globals.hh"
#include "G4MagneticField.hh"

class G4GenericMessenger;

/// Magnetic field

class MagneticField : public G4MagneticField
{
public:
  MagneticField();
  virtual ~MagneticField();

  virtual void GetFieldValue(const G4double point[4], double *bField) const;

  void SetField(G4double val) { fBx = val; }
  G4double GetField() const { return fBx; }

private:
  void DefineCommands();

  G4GenericMessenger *fMessenger;
  G4double fBx;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
