/// \file GasChamberHit.hh
/// \brief Definition of the GasChamberHit class

#ifndef GasChamberHit_h
#define GasChamberHit_h 1

#include <vector>

#include "G4DynamicParticle.hh"
#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
///
/// It records:
/// - the energy deposit 

class GasChamberHit : public G4VHit
{
    public:
    GasChamberHit();
    GasChamberHit(const G4DynamicParticle *pDynamic, G4int evtId, G4int trkId);
    GasChamberHit(const GasChamberHit &right);
    virtual ~GasChamberHit();

    const GasChamberHit &operator=(const GasChamberHit &right);
    G4bool operator==(const GasChamberHit &right) const;

    inline void *operator new(size_t);
    inline void operator delete(void *aHit);

    virtual void Print();
    std::vector<G4double> GetEdep() const { return fEdep; }
    std::vector<G4double> GetTime() const { return fTime; }
    std::vector<G4double> GetPosX() const { return fPosX; }
    std::vector<G4double> GetPosY() const { return fPosY; }
    std::vector<G4double> GetPosZ() const { return fPosZ; }
    std::vector<G4double> GetMomX() const { return fMomX; }
    std::vector<G4double> GetMomY() const { return fMomY; }
    std::vector<G4double> GetMomZ() const { return fMomZ; }
    std::vector<G4double> GetCharge() const {return fCharge;}
    G4double GetEdepSum() const { return fEdepSum; }
    G4double GetTrackLength() const {return fTrackLen;}
    G4int GetTrackId() const {return fTrackId;}
    G4int GetEventId() const {return fEventId;}
    G4int GetAtomicNumber() const {return fZ;}
    G4double GetMass() const {return fMass;}
    G4String GetPartName() const {return fPartName;}
    G4int GetNbOfStepPoints() const {return fNbOfStepPoints;}

    // to save information
    void AppendEdep(G4double de);
    void AppendTime(G4double t);
    void AppendCharge(G4double q);
    void AppendPosition(const G4ThreeVector &pos);
    void AppendMomentum(const G4ThreeVector &mom);
    void AddEdepSum(G4double de);
    void AddTrackLength(G4double leng);
    void SetTrackId(G4double id);
    void SetEventId(G4double id);
    void SetAtomicNumber(G4double z);
    void SetMass(G4double mass);
    void SetPartName(const G4String &name);
    void SetNbOfStepPoints(G4int nSteps);

    private:
    // by steps
    std::vector<G4double> fPosX, fPosY, fPosZ, fMomX, fMomY, fMomZ, fEdep, fTime, fCharge;
    
    // by tracks
    G4int fEventId, fTrackId, fZ, fNbOfStepPoints;
    G4double fEdepSum, fTrackLen, fMass;
    G4String fPartName;
};

using GasChamberHitsCollection = G4THitsCollection<GasChamberHit>;

extern G4ThreadLocal G4Allocator<GasChamberHit> *GasChamberHitAllocator;

inline void *GasChamberHit::operator new(size_t)
{
    if(!GasChamberHitAllocator)
    {
        GasChamberHitAllocator = new G4Allocator<GasChamberHit>;
    }
    return (void *)GasChamberHitAllocator->MallocSingle();
}

inline void GasChamberHit::operator delete(void *aHit)
{
    GasChamberHitAllocator->FreeSingle((GasChamberHit *)aHit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
