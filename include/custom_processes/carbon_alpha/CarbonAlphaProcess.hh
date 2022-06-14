/// \file CarbonAlphaProcess.hh
/// \brief Definition of the CarbonAlphaProcess class

#ifndef CarbonAlphaProcess_h
#define CarbonAlphaProcess_h 1

#include "custom_processes/carbon_alpha/CarbonAlphaProcessMessenger.hh"
#include "custom_processes/mean_free_path/MeanFreePathEval.hh"

#include "G4GenericMessenger.hh"
#include "G4VDiscreteProcess.hh"
#include "G4IonTable.hh"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include <memory>
#include <vector>
#include <array>

#include "TGenPhaseSpace.h"

class CarbonAlphaProcessMessenger;

/// This process forces interaction if a carbon beam energy is smaller than a given value.
class CarbonAlphaProcess final : public G4VDiscreteProcess
{
    public:
    CarbonAlphaProcess(G4String name = "CarbonAlpha", const G4Region *region = nullptr);
    virtual ~CarbonAlphaProcess() override;

    // overriding pure virtual methods
    virtual G4bool IsApplicable(const G4ParticleDefinition &Def) override;
    virtual G4VParticleChange *PostStepDoIt(const G4Track &aTrack, const G4Step &aStep) override;
    virtual G4double PostStepGetPhysicalInteractionLength(
        const G4Track &track, G4double previousStepSize, G4ForceCondition *condition) override;
    virtual G4double GetMeanFreePath(const G4Track &aTrack, G4double previousStepSize, G4ForceCondition *condition) override;

    CarbonAlphaProcess& operator=(const CarbonAlphaProcess &right) = delete;
    CarbonAlphaProcess(const CarbonAlphaProcess&) = delete;

    void Activate(G4bool enable = true);
    void ApplyOnlyPrimary(G4bool onlyPrimary = true);

    void ForceAtKinE(G4double kinE);
    void ForceAtTrkLen(G4double trkLen);
    void ForceAtRndmTrkLenUniform(G4double trkLenMin, G4double trkLenMax);
    void SetReactionRegion(const G4Region *region)
    {
        reactionRegion = region;
        meanFreePathEval->SetReactionRegion(reactionRegion);
    }
    void SetOxygenCharge(G4double charge) { fOxygenCharge = charge; }
    
    protected:
    G4bool CheckApplicable(const G4Track *track) const;
    void StartTracking(G4Track *track) override;
    void EndTracking() override;
    private:
    // generate random momentom vector whose magnitude follows Maxwell-Moltzmann dist.
    // non relativistic
    G4ThreeVector GenerateMomInTempNR(const G4ParticleDefinition *pDef, G4double tepm = STP_Temperature);

    void GeneratePhases(const G4Track &aTrack);
    void InitDynamicDaughters();
    void InitParticleChange(const G4Track &aTrack);
    private:
    std::unique_ptr<MeanFreePathEval> meanFreePathEval;
    G4bool activated;
    G4bool onlyPrimary;
    G4Region const *reactionRegion;
    
    TGenPhaseSpace *fGenPhaseSpace;
    
    G4DynamicParticle *fDynamicOxygen, *fDynamicGamma;
    G4double fOxygenCharge;

    const G4ParticleDefinition *kDefAlpha, *kDefOxygen, *kDefGamma;
    const G4double daughterMasses[2];

    std::unique_ptr<CarbonAlphaProcessMessenger> messenger;
    // The base unit of ROOT is GeV(GeV/c) and MeV(MeV/c) for Geant4;
    static constexpr G4double energyUnitCnv = MeV/GeV;
    static constexpr G4int kIonAtomicMass = 12;
    static constexpr G4int kIonAtomicNum = 6;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
