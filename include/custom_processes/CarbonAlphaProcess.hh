/// \file CarbonAlphaProcess.hh
/// \brief Definition of the CarbonAlphaProcess class

#ifndef CarbonAlphaProcess_h
#define CarbonAlphaProcess_h 1

#include "G4GenericMessenger.hh"
#include "G4VDiscreteProcess.hh"
#include "G4IonTable.hh"

#include <vector>
#include <array>


/// This process forces interaction if a carbon beam energy is smaller than a given value.
class CarbonAlphaProcess final : public G4VDiscreteProcess
{
    public:
    CarbonAlphaProcess(G4String name = "CarbonAlpha");
    virtual ~CarbonAlphaProcess() override;

    // overriding pure virtual methods
    virtual G4bool IsApplicable(const G4ParticleDefinition &Def) override;
    
    // virtual void PreparePhysicsTable(const G4ParticleDefinition &pDef) override;
    
    virtual G4VParticleChange *PostStepDoIt(const G4Track &aTrack, const G4Step &aStep) override;
    
    virtual G4double PostStepGetPhysicalInteractionLength(
        const G4Track &track, G4double previousStepSize, G4ForceCondition *condition) override;
    
    virtual G4double GetMeanFreePath(const G4Track &track, G4double previousStepSize, G4ForceCondition *condition);

    CarbonAlphaProcess& operator=(const CarbonAlphaProcess &right) = delete;
    CarbonAlphaProcess(const CarbonAlphaProcess&) = delete;
    
    void SetHeliumCharge(G4double charge){fAlphaCharge = charge;}
    // forced reactions
    protected:
    
    private:
        void DefineCommands();
    private:
        G4DynamicParticle *fDynamicAlpha, *fDynamicGamma;
        static constexpr G4int kIonAtomicMass = 12;
        static constexpr G4int kIonAtomicNum = 6;
        const G4ParticleDefinition *kDefAlpha, *kDefGamma;
        G4double fAlphaCharge;
        G4GenericMessenger *fMessenger;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
