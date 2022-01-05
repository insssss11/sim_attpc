/// \file CarbonAlphaProcess.cc
/// \brief Implementation of the CarbonAlphaProcess class

#include "custom_processes/CarbonAlphaProcess.hh"
#include "G4Alpha.hh"
#include "G4Gamma.hh"
#include "G4SystemOfUnits.hh"
#include "G4HadronicProcessStore.hh"

CarbonAlphaProcess::CarbonAlphaProcess(G4String name)
    : G4VDiscreteProcess(name, fHadronic),
    fDynamicAlpha(nullptr), fDynamicGamma(nullptr),
    kDefAlpha(G4Alpha::Definition()), kDefGamma(G4Gamma::Definition()),
    fAlphaCharge(2.),
    fMessenger(nullptr)
{
    SetProcessSubType(fCapture);
    DefineCommands();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CarbonAlphaProcess::~CarbonAlphaProcess()
{
    delete fMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool CarbonAlphaProcess::IsApplicable(const G4ParticleDefinition &Def)
{
    return Def.GetParticleName() == "GenericIon";
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VParticleChange *CarbonAlphaProcess::PostStepDoIt(const G4Track &aTrack, const G4Step &aStep)
{
    aParticleChange.Initialize(aTrack);
    aParticleChange.SetNumberOfSecondaries(2);
    fDynamicAlpha = new G4DynamicParticle(kDefAlpha, G4ThreeVector(0., 10.*MeV, 0.));
    fDynamicAlpha->SetCharge(fAlphaCharge);
    fDynamicGamma = new G4DynamicParticle(kDefGamma, G4ThreeVector(0., -10.*MeV, 0.));
    aParticleChange.AddSecondary(fDynamicAlpha);
    aParticleChange.AddSecondary(fDynamicGamma);
    aParticleChange.ProposeLocalEnergyDeposit(0.);
    aParticleChange.ProposeTrackStatus(fStopAndKill);
    return pParticleChange;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double CarbonAlphaProcess::PostStepGetPhysicalInteractionLength(
    const G4Track &track, G4double previousStepSize, G4ForceCondition *condition)
{
    if((previousStepSize <= 0.0) || (theNumberOfInteractionLengthLeft <= 0.0))
        ResetNumberOfInteractionLengthLeft();
    else{
        SubtractNumberOfInteractionLengthLeft(previousStepSize);
        if(theNumberOfInteractionLengthLeft < perMillion)
            theNumberOfInteractionLengthLeft = 0;
    }
    currentInteractionLength = GetMeanFreePath(track, previousStepSize, condition);
    G4double value = theNumberOfInteractionLengthLeft*currentInteractionLength;
    return value;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double CarbonAlphaProcess::GetMeanFreePath(const G4Track &track, G4double previousStepSize, G4ForceCondition *condition)
{
    if(track.GetDynamicParticle()->GetDefinition()->GetParticleName() != "C12")
        *condition = InActivated;
    if(track.GetKineticEnergy() < 9.0*MeV)
    {
        *condition = Forced;
        return 0;
    }
    else
    {
        *condition = NotForced;
        return DBL_MAX;
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CarbonAlphaProcess::DefineCommands()
{
    fMessenger = new G4GenericMessenger(this, "/attpc/process/CarbonAlpha/", "Carbon alpha process control");
}