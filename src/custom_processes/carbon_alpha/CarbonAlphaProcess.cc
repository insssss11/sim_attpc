/// \file CarbonAlphaProcess.cc
/// \brief Implementation of the CarbonAlphaProcess class

#include "custom_processes/carbon_alpha/CarbonAlphaProcess.hh"

#include "custom_processes/mean_free_path/MeanFreePathByKinE.hh"
#include "custom_processes/mean_free_path/MeanFreePathByKinEuniform.hh"
#include "custom_processes/mean_free_path/MeanFreePathByTrkLen.hh"
#include "custom_processes/mean_free_path/MeanFreePathByTrkLenUniform.hh"

#include "G4IonTable.hh"
#include "G4Gamma.hh"
#include "G4Alpha.hh"
#include "G4HadronicProcessStore.hh"
#include "G4RegionStore.hh"

CarbonAlphaProcess::CarbonAlphaProcess(G4String name, const G4Region *region)
    : G4VDiscreteProcess(name, fHadronic),
    activated(true),
    reactionRegion(region),
    fGenPhaseSpace(nullptr),
    fDynamicOxygen(nullptr), fDynamicGamma(nullptr),
    fOxygenCharge(8.),
    kDefAlpha(G4Alpha::Definition()),
    kDefOxygen(G4IonTable::GetIonTable()->GetIon(8, 16)),
    kDefGamma(G4Gamma::Definition()),
    daughterMasses{
        kDefOxygen->GetPDGMass()*energyUnitCnv,
        kDefGamma->GetPDGMass()*energyUnitCnv}
{
    if(reactionRegion == nullptr)
        reactionRegion = G4RegionStore::GetInstance()->GetRegion("DefaultRegionForTheWorld");
    SetVerboseLevel(0);
    messenger = std::make_unique<CarbonAlphaProcessMessenger>(this);    
    fGenPhaseSpace = new TGenPhaseSpace();
    SetProcessSubType(fCapture);
    ForceAtKinE(8.0*MeV);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CarbonAlphaProcess::~CarbonAlphaProcess()
{
    delete fGenPhaseSpace;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CarbonAlphaProcess::Activate(G4bool enable)
{
    activated = enable;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool CarbonAlphaProcess::IsApplicable(const G4ParticleDefinition &def)
{
    return def.GetParticleName() == "GenericIon";
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VParticleChange *CarbonAlphaProcess::PostStepDoIt(const G4Track &aTrack, const G4Step &)
{
    GeneratePhases(aTrack);
    InitDynamicDaughters();
    InitParticleChange(aTrack);
    return pParticleChange;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CarbonAlphaProcess::GeneratePhases(const G4Track &aTrack)
{
    // generate random value for alpha particle(Not implemented yet)
    G4ThreeVector pAlpha = GenerateMomInTempNR(kDefAlpha);
    G4double p1, p2, p3, p4;
    p1 = aTrack.GetMomentum().getX() + pAlpha.getX();
    p2 = aTrack.GetMomentum().getY() + pAlpha.getY();
    p3 = aTrack.GetMomentum().getZ() + pAlpha.getZ();
    p4 = aTrack.GetTotalEnergy() + kDefAlpha->GetPDGMass();
    TLorentzVector lCarbonAlpha(p1*energyUnitCnv, p2*energyUnitCnv, p3*energyUnitCnv, p4*energyUnitCnv);
    // init and generate
    fGenPhaseSpace->SetDecay(lCarbonAlpha, 2, daughterMasses);
    fGenPhaseSpace->Generate();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CarbonAlphaProcess::InitDynamicDaughters()
{
    TLorentzVector *lv;
    lv = fGenPhaseSpace->GetDecay(0);
    fDynamicOxygen = new G4DynamicParticle(kDefOxygen,
        G4ThreeVector(lv->Px()/energyUnitCnv, lv->Py()/energyUnitCnv, lv->Pz()/energyUnitCnv));
    fDynamicOxygen->SetCharge(fOxygenCharge);
    lv = fGenPhaseSpace->GetDecay(1);
    fDynamicGamma = new G4DynamicParticle(kDefGamma,
        G4ThreeVector(lv->Px()/energyUnitCnv, lv->Py()/energyUnitCnv, lv->Pz()/energyUnitCnv));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CarbonAlphaProcess::InitParticleChange(const G4Track &aTrack)
{
    aParticleChange.Initialize(aTrack);
    aParticleChange.SetNumberOfSecondaries(2);
    aParticleChange.AddSecondary(fDynamicOxygen);
    aParticleChange.AddSecondary(fDynamicGamma);
    aParticleChange.ProposeLocalEnergyDeposit(0.);
    aParticleChange.ProposeTrackStatus(fStopAndKill);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double CarbonAlphaProcess::PostStepGetPhysicalInteractionLength(
    const G4Track &track, G4double previousStepSize, G4ForceCondition *condition)
{
    if(!activated || track.GetParticleDefinition()->GetParticleName() != "C12")
    {
        *condition = InActivated;
        return DBL_MAX;
    }
    if((previousStepSize <= 0.0) || (theNumberOfInteractionLengthLeft <= 0.0))
        ResetNumberOfInteractionLengthLeft();
    else
    {
        SubtractNumberOfInteractionLengthLeft(previousStepSize);
        if(theNumberOfInteractionLengthLeft < perMillion)
            theNumberOfInteractionLengthLeft = 0;
    }
    currentInteractionLength = GetMeanFreePath(track, previousStepSize, condition);
    G4double value = theNumberOfInteractionLengthLeft*currentInteractionLength;
    return value;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4double CarbonAlphaProcess::GetMeanFreePath(const G4Track &aTrack, G4double previousStepSize, G4ForceCondition *condition)
{
    return meanFreePathEval->Eval(&aTrack, previousStepSize, condition);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreeVector CarbonAlphaProcess::GenerateMomInTempNR(const G4ParticleDefinition *, G4double)
{
    // not yet implemented
    return G4ThreeVector(0., 0., 0.);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CarbonAlphaProcess::StartTracking(G4Track *)
{
    meanFreePathEval->StartOfTrack();
    theNumberOfInteractionLengthLeft = -1.0;
    currentInteractionLength = -1.0;
    theInitialNumberOfInteractionLength = -1.0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CarbonAlphaProcess::EndTracking()
{
    meanFreePathEval->EndOfTrack();
    theNumberOfInteractionLengthLeft = -1.0;
    currentInteractionLength = -1.0;
    theInitialNumberOfInteractionLength = -1.0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CarbonAlphaProcess::ForceAtKinE(G4double kinE)
{
    auto mfpe = new MeanFreePathByKinE();
    mfpe->SetKinE(kinE);
    meanFreePathEval.reset();
    meanFreePathEval = std::unique_ptr<MeanFreePathByKinE>(mfpe);
    meanFreePathEval->SetReactionRegion(reactionRegion);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CarbonAlphaProcess::ForceAtTrkLen(G4double len)
{
    auto mfpe = new MeanFreePathByTrkLen();
    mfpe->SetTrkLen(len);
    meanFreePathEval.reset();
    meanFreePathEval = std::unique_ptr<MeanFreePathByTrkLen>(mfpe);
    meanFreePathEval->SetReactionRegion(reactionRegion);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CarbonAlphaProcess::ForceAtRndmTrkLenUniform(G4double lenMin, G4double lenMax)
{
    auto mfpe = new MeanFreePathByTrkLenUniform();
    mfpe->SetRange(lenMin, lenMax);
    meanFreePathEval.reset();
    meanFreePathEval = std::unique_ptr<MeanFreePathByTrkLenUniform>(mfpe);
    meanFreePathEval->SetReactionRegion(reactionRegion);
}
