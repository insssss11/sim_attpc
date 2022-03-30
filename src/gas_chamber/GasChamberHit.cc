/// \file GasChamberHit.cc
/// \brief Implementation of the GasChamberHit class

#include "gas_chamber/GasChamberHit.hh"

#include "G4SystemOfUnits.hh"
#include "G4ios.hh"
#include "G4UnitsTable.hh"
#include "G4PrimaryParticle.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal G4Allocator<GasChamberHit> *GasChamberHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GasChamberHit::GasChamberHit()
    : G4VHit(),
    fEventId(-1), fTrackId(-1), fZ(0),
    fEdepSum(0), fTrackLen(0)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GasChamberHit::GasChamberHit(const G4DynamicParticle *pDynamic, G4int evtId, G4int trkId)
    : G4VHit(),
    fEdepSum(0), fTrackLen(0), fMass(0), fPartName()
{
    auto pDef = pDynamic->GetDefinition();
    SetPartName(pDef->GetParticleName());
    SetMass(pDef->GetPDGMass());
    SetAtomicNumber(pDef->GetAtomicNumber());
    SetEventId(evtId);
    SetTrackId(trkId);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GasChamberHit::~GasChamberHit()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

GasChamberHit::GasChamberHit(const GasChamberHit &right)
    : G4VHit()
{
    fPosX = right.GetPosX();
    fPosY = right.GetPosY();
    fPosZ = right.GetPosZ();
    fMomX = right.GetMomX();
    fMomY = right.GetMomY();
    fMomZ = right.GetMomZ();
    fEdep = right.GetEdep();
    fTime = right.GetTime();
    fCharge = right.GetCharge();
    fEventId = right.GetEventId();
    fZ = right.GetAtomicNumber();
    fNbOfStepPoints = right.GetNbOfStepPoints();
    fEdepSum = right.GetEdepSum();
    fTrackLen = right.GetTrackLength();
    fMass = right.GetMass();
    fPartName = right.GetPartName();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const GasChamberHit &GasChamberHit::operator=(const GasChamberHit &right)
{
    fPosX = right.GetPosX();
    fPosY = right.GetPosY();
    fPosZ = right.GetPosZ();
    fMomX = right.GetMomX();
    fMomY = right.GetMomY();
    fMomZ = right.GetMomZ();
    fEdep = right.GetEdep();
    fTime = right.GetTime();
    fCharge = right.GetCharge();
    fEventId = right.GetEventId();
    fZ = right.GetAtomicNumber();
    fNbOfStepPoints = right.GetNbOfStepPoints();
    fEdepSum = right.GetEdepSum();
    fTrackLen = right.GetTrackLength();
    fMass = right.GetMass();
    fPartName = right.GetPartName();
    return *this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool GasChamberHit::operator==(const GasChamberHit &right) const
{
    return fPosX == right.GetPosX() &&
    fPosY == right.GetPosY() &&
    fPosZ == right.GetPosZ() &&
    fMomX == right.GetMomX() &&
    fMomY == right.GetMomY() &&
    fMomZ == right.GetMomZ() &&
    fEdep == right.GetEdep() &&
    fTime == right.GetTime() &&
    fCharge == right.GetCharge() &&
    fEventId == right.GetEventId() &&
    fZ == right.GetAtomicNumber() &&
    fNbOfStepPoints == right.GetNbOfStepPoints() &&
    fEdepSum == right.GetEdepSum() &&
    fTrackLen == right.GetTrackLength() &&
    fMass == right.GetMass() &&
    fPartName == right.GetPartName();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberHit::Print()
{
    for(int j = 0;j < GetNbOfStepPoints();++j)
    {
        G4ThreeVector mom(fMomX.at(j), fMomY.at(j), fMomZ.at(j));
        G4cout << std::setw(10) << std::right << G4BestUnit(fPosX.at(j), "Length")
            << std::setw(10) << G4BestUnit(fPosY.at(j), "Length")
            << std::setw(10) << G4BestUnit(fPosZ.at(j), "Length")
            << std::setw(12) << mom.getX()/mom.mag()
            << std::setw(10) << mom.getY()/mom.mag()
            << std::setw(10) << mom.getZ()/mom.mag()
            << std::setw(10) << G4BestUnit(fEdep.at(j), "Energy")
            << std::setw(10) << G4BestUnit(sqrt(mom.mag2() + fMass*fMass) - fMass, "Energy")
            << std::setw(10) << fCharge.at(j) << G4endl;
    }
    G4cout << "--------------------------------------------------------------------------------------------------------------------------------" << G4endl;
    G4cout << std::setw(40) << std::left << "Total Track Length : " << std::setw(10) << std::right << G4BestUnit(fTrackLen, "Length") << G4endl;
    G4cout << std::setw(40) << std::left << "Total Energy Deposit : " << std::setw(10) << std::right << G4BestUnit(fEdepSum, "Energy") << G4endl;
    G4cout << "--------------------------------------------------------------------------------------------------------------------------------" << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberHit::AppendEdep(G4float de)
{
    fEdep.push_back(de);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberHit::AppendTime(G4float t)
{
    fTime.push_back(t);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberHit::AppendCharge(G4float q)
{
    fCharge.push_back(q);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberHit::AppendPosition(const G4ThreeVector &pos)
{
    fPosX.push_back(pos[0]);
    fPosY.push_back(pos[1]);
    fPosZ.push_back(pos[2]);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberHit::AppendMomentum(const G4ThreeVector &mom)
{
    fMomX.push_back(mom[0]);
    fMomY.push_back(mom[1]);
    fMomZ.push_back(mom[2]);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberHit::AppendStepLen(G4float stepLen)
{
    fStepLen.push_back(stepLen);
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberHit::AddEdepSum(G4float de)
{
    fEdepSum += de;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberHit::AddTrackLength(G4float leng)
{
    fTrackLen += leng;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberHit::SetTrackId(G4float trkId)
{
    fTrackId = trkId;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberHit::SetEventId(G4float evtId)
{
    fEventId = evtId;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberHit::SetAtomicNumber(G4float z)
{
    fZ = z;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberHit::SetMass(G4float mass)
{
    fMass = mass;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberHit::SetPartName(const G4String &name)
{
    fPartName = name;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void GasChamberHit::SetNbOfStepPoints(G4int nStep)
{
    fNbOfStepPoints = nStep;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......