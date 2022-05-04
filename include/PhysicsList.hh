#ifndef PhysicsList_h
#define PhysicsList_h 1

#include "G4VModularPhysicsList.hh"
#include "G4GenericMessenger.hh"

class G4VPhysicsConstructor;
class PhysicsListMessenger;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class PhysicsList : public G4VModularPhysicsList
{
    public:

    PhysicsList();
    ~PhysicsList();

    virtual void ConstructParticle();
    virtual void ConstructProcess();

    private:
    void ActivateDeltaRay(G4bool activate = true);
    void ActivateRecoilNuclei(G4bool activate = true);

    void AddIonGasProcess();
    void AddCarbonAlphaProcess();
    void AddLimiterProcess();

    void DefineCommands();

    private:
    G4GenericMessenger *fMessenger;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

