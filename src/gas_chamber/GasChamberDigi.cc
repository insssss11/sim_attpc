/// \file GasChamberDigi.cc
/// \brief Implementation of the GasChamberDigi class

#include "gas_chamber/GasChamberDigi.hh"

#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "G4UnitsTable.hh"
#include "G4PrimaryParticle.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal G4Allocator<GasChamberDigi> *GasChamberDigiAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
GasChamberDigi::GasChamberDigi()
    : G4VDigi()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GasChamberDigi::GasChamberDigi(const GasChamberDigi& right)
    : G4VDigi(), padNum(right.GetPadNum()), charge(right.GetCharge())
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GasChamberDigi::~GasChamberDigi()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const GasChamberDigi &GasChamberDigi::operator=(const GasChamberDigi &right)
{
    padNum = right.GetPadNum();
    charge = right.GetCharge();
    return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool GasChamberDigi::operator==(const GasChamberDigi &right) const
{
    return  padNum == right.GetPadNum() &&
        charge == right.GetCharge();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberDigi::SetPadNum(G4double num)
{
    padNum = num;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberDigi::AddCharge(G4double _charge)
{
    charge += _charge;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberDigi::Clear()
{
    charge = 0.;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberDigi::Draw()
{
    
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberDigi::Print()
{
}

