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
GasChamberDigi::GasChamberDigi(const G4TwoVector &pos, const G4double xHalf, const G4double yHalf, G4double margin)
    : G4VDigi(), pos(pos), xHalf(xHalf), yHalf(yHalf), margin(margin)
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GasChamberDigi::GasChamberDigi(const GasChamberDigi& right)
    : GasChamberDigi(right.pos, right.xHalf, right.yHalf)
{
    padNum = right.GetPadNum();
    charge = right.GetCharge();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GasChamberDigi::~GasChamberDigi()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const GasChamberDigi &GasChamberDigi::operator=(const GasChamberDigi &right)
{
    padNum = right.GetPadNum();
    charge = right.GetCharge();
    timeWeightedSum = right.timeWeightedSum;
    return *this;
}

const G4TwoVector GasChamberDigi::GetPosition() const
{
    return pos;
}

void GasChamberDigi::GetRange(G4double *min, G4double *max) const
{
    min[0] = pos[0] - xHalf + margin;
    min[1] = pos[1] - yHalf + margin;

    max[0] = pos[0] + xHalf - margin;
    max[1] = pos[1] + yHalf - margin; 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool GasChamberDigi::operator==(const GasChamberDigi &right) const
{
    return
        pos == right.pos &&
        xHalf == right.xHalf &&
        yHalf == right.yHalf &&
        padNum == right.GetPadNum() &&
        charge == right.GetCharge() &&
        timeWeightedSum == right.timeWeightedSum;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberDigi::SetPadNum(G4int num)
{
    padNum = num;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberDigi::SetMargin(G4double margin)
{
    this->margin = margin;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberDigi::AddWeightedTime(G4double time, G4double charge)
{
    timeWeightedSum += time*charge;
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
    timeWeightedSum = 0.;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberDigi::Draw()
{

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberDigi::Print()
{
}

