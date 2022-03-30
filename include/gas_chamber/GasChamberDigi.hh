#ifndef GasChamberDigi_h
#define GasChamberDigi_h 1

#include "G4VDigi.hh"
#include "G4TDigiCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

// This Class describe the pads of the at-tpc
class GasChamberDigi : public G4VDigi
{
    public:
    GasChamberDigi();
    GasChamberDigi(const GasChamberDigi&);
    ~GasChamberDigi();

    const GasChamberDigi& operator=(const GasChamberDigi&);
    G4bool operator==(const GasChamberDigi&) const;
    inline void* operator new(size_t);
    inline void  operator delete(void*);

    G4int GetPadNum() const { return padNum; }
    G4float GetCharge() const { return charge; }

    void SetPadNum(int num);
    void AddCharge(G4float _charge);

    void Clear();
    void Draw();
    void Print();
    private:
    G4int padNum;
    G4float charge;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

typedef G4TDigiCollection<GasChamberDigi> GasChamberDigitsCollection;

extern G4ThreadLocal G4Allocator<GasChamberDigi> *GasChamberDigiAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

inline void* GasChamberDigi::operator new(size_t)
{
    if(!GasChamberDigiAllocator)
        GasChamberDigiAllocator = new G4Allocator<GasChamberDigi>;
    return (void*)GasChamberDigiAllocator->MallocSingle();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

inline void GasChamberDigi::operator delete(void* aDigi)
{
    GasChamberDigiAllocator->FreeSingle((GasChamberDigi*)aDigi);
}

#endif









