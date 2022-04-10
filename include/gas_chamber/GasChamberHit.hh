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
#include "training_data/TrainingDataTypes.hh"

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
    std::vector<G4float> GetEdep() const { return fEdep; }
    std::vector<G4float> GetTime() const { return fTime; }
    std::vector<G4float> GetPosX() const { return fPosX; }
    std::vector<G4float> GetPosY() const { return fPosY; }
    std::vector<G4float> GetPosZ() const { return fPosZ; }
    std::vector<G4float> GetMomX() const { return fMomX; }
    std::vector<G4float> GetMomY() const { return fMomY; }
    std::vector<G4float> GetMomZ() const { return fMomZ; }
    std::vector<G4float> GetCharge() const {return fCharge;}
    std::vector<G4float> GetStepLen() const {return fStepLen;}
    G4float GetEdepSum() const { return fEdepSum; }
    G4float GetTrackLength() const {return fTrackLen;}
    G4int GetTrackId() const {return fTrackId;}
    G4int GetEventId() const {return fEventId;}
    TrainingDataTypes::EParticle GetParticleEnum() const {return parEnum;}
    G4float GetMass() const {return fMass;}
    G4String GetPartName() const {return fPartName;}
    G4int GetNbOfStepPoints() const {return fNbOfStepPoints;}

    // to save information
    void AppendEdep(G4float de);
    void AppendTime(G4float t);
    void AppendCharge(G4float q);
    void AppendPosition(const G4ThreeVector &pos);
    void AppendMomentum(const G4ThreeVector &mom);
    void AppendStepLen(G4float stepLen);
    void AddEdepSum(G4float de);
    void AddTrackLength(G4float leng);
    void SetTrackId(G4float trkId);
    void SetEventId(G4float evtId);
    void SetParticleEnum(TrainingDataTypes::EParticle parEnum);
    void SetMass(G4float mass);
    void SetPartName(const G4String &name);
    void SetNbOfStepPoints(G4int nSteps);

    private:
    // by steps
    std::vector<G4float> fPosX, fPosY, fPosZ, fMomX, fMomY, fMomZ, fEdep, fTime, fCharge, fStepLen;
    TrainingDataTypes::EParticle parEnum;
    // by tracks
    G4int fEventId, fTrackId, fNbOfStepPoints;
    G4float fEdepSum, fTrackLen, fMass;
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
