/// \file GasChamberDigitizer.cc
/// \brief Implementation of the GasChamberDigitizer class

#include "gas_chamber/GasChamberDigitizer.hh"
#include "gas_chamber/GasChamberHit.hh"

#include "G4SDManager.hh"
#include "G4RunManager.hh"

#include "G4ios.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GasChamberDigitizer::GasChamberDigitizer(G4String name,
    G4double _padPlaneX, G4double _padPlaneY, G4int _nPadX, G4int _nPadY,
    G4double eIonPair, const G4ThreeVector &_centerPos)
    : G4VDigitizerModule(name),
    padPlaneX(_padPlaneX), padPlaneY(_padPlaneY), nPadX(_nPadX), nPadY(_nPadY),
    padMargin(0.),
    energyPerElectronPair(eIonPair), centerPos(_centerPos),
    readoutPads(nullptr)
{
    InitPads();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GasChamberDigitizer::~GasChamberDigitizer()
{
    delete readoutPads;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const std::vector<G4double> GasChamberDigitizer::GetChargeOnPads() const
{
    std::vector<G4double> charge;
    charge.reserve(nPadX*nPadY);
    for(const auto &yPads : *readoutPads)
        for(const auto &pad : yPads)
            charge.push_back(pad.GetCharge());
    return charge;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberDigitizer::Digitize()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberDigitizer::ClearPads()
{
    for(auto &yPads : *readoutPads)
        for(auto &pad : yPads)
            pad.Clear();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberDigitizer::FillPadsInfo(const GasChamberHitsCollection* hitsCollection)
{
    for(size_t trk = 0;trk < hitsCollection->GetSize();++trk)
        FillPadsTrack(static_cast<const GasChamberHit*>(hitsCollection->GetHit(trk)));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberDigitizer::FillPadsTrack(const GasChamberHit *hit)
{
    const std::vector<G4double> eDeps = hit->GetEdep();
    const std::vector<G4double>
        xPos = hit->GetPosX(),
        yPos = hit->GetPosY(),
        zPos = hit->GetPosZ();
    for(int step = 0;step < hit->GetNbOfStepPoints();++step)
        FillPadsStep(
        G4ThreeVector(xPos.at(step), yPos.at(step), zPos.at(step)),
        eDeps.at(step));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberDigitizer::FillPadsStep(const G4ThreeVector &ePos, G4double eDep)
{
    // coordinate in the frame of gas chamber
    const G4double ePosX = ePos.getZ(), ePosY = ePos.getY(), ePosZ = -ePos.getZ();
    const G4double
        minPadPlaneX = centerPos.getX() - padPlaneX/2,
        minPadPlaneY = centerPos.getY() - padPlaneY/2,
        padX = padPlaneX/nPadX,
        padY = padPlaneY/nPadY;
    if(ePosX < minPadPlaneX || ePosX > minPadPlaneX + padPlaneX ||
        ePosY < minPadPlaneY || ePosY > minPadPlaneY + padPlaneY)
        return;
    int padNumX = 0, padNumY = 0;
    for(padNumX = 0;padNumX < nPadX;++padNumX)
    {
        if(ePosX < minPadPlaneX + (padNumX + 1)*padX)
            break;
    }
    for(padNumY = 0;padNumY < nPadY;++padNumY)
    {
        if(ePosY < minPadPlaneY + (padNumY + 1)*padY)
            break;
    }
    // charge in fC
    G4double charge = 1e12*e_SI*chargeGain*eDep/energyPerElectronPair;
    readoutPads->at(padNumY).at(padNumX).AddCharge(charge);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberDigitizer::SetChargeMultiplication(G4double gain)
{
    chargeGain = gain;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberDigitizer::SetThreshold(G4double threshold)
{
    adcThreshold = threshold;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberDigitizer::SetPadPlaneCenter(const G4ThreeVector &pos)
{
    centerPos = pos;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberDigitizer::SetPadMargin(G4double margin)
{
    padMargin = margin;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberDigitizer::InitPads()
{
    if(readoutPads != nullptr)
        delete readoutPads;
    readoutPads = new std::vector<std::vector<GasChamberDigi>>;
    readoutPads->reserve(nPadY);
    for(int y = 0;y < nPadY;++y)
    {
        readoutPads->push_back(std::vector<GasChamberDigi>(nPadX));
        for(int x = 0;x < nPadX;++x)
            readoutPads->at(y).at(x).SetPadNum(x + y*nPadX);
    }
}