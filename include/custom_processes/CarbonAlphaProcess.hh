/// \file CarbonAlphaProcess.hh
/// \brief Definition of the CarbonAlphaProcess class

#ifndef CarbonAlphaProcess_h
#define CarbonAlphaProcess_h 1

#include "G4GenericMessenger.hh"
#include "G4VDiscreteProcess.hh"
#include "G4IonTable.hh"
#include "G4PhysicalConstants.hh"

#include <vector>
#include <array>

#include "TGenPhaseSpace.h"

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
    
    void SetOxygenCharge(G4double charge){fOxygenCharge = charge;}
    protected:
        G4int verboseLevel;
    private:
        // generate random momentom vector whose magnitude follows Maxwell-Moltzmann dist.
        // non relativistic
        G4ThreeVector GenerateMomInTempNR(const G4ParticleDefinition *pDef, G4double tepm = STP_Temperature);

        // to be used for UI commands
        void ForceReactionByKinE(G4double kinE);
        void ForceReactionByTrackLen(G4double length);
        void DefineCommands();
    private:
        TGenPhaseSpace *fGenPhaseSpace;
        G4DynamicParticle *fDynamicOxygen, *fDynamicGamma;
        G4double fOxygenCharge;
        G4double fEnergyOfReaction, fTrackLenOfReaction;
        G4GenericMessenger *fMessenger;
        const G4ParticleDefinition *kDefAlpha, *kDefOxygen, *kDefGamma;

        static constexpr G4int kIonAtomicMass = 12;
        static constexpr G4int kIonAtomicNum = 6;        
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
