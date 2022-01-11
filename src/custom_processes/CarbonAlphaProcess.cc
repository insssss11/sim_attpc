/// \file CarbonAlphaProcess.cc
/// \brief Implementation of the CarbonAlphaProcess class

#include "custom_processes/CarbonAlphaProcess.hh"
#include "G4IonTable.hh"
#include "G4Gamma.hh"
#include "G4Alpha.hh"
#include "G4SystemOfUnits.hh"
#include "G4HadronicProcessStore.hh"

CarbonAlphaProcess::CarbonAlphaProcess(G4String name)
    : G4VDiscreteProcess(name, fHadronic),
    verboseLevel(0),
    fGenPhaseSpace(nullptr),
    fDynamicOxygen(nullptr), fDynamicGamma(nullptr),
    fOxygenCharge(8.),
    fEnergyOfReaction(-1.), fTrackLenOfReaction(-1.),
    fMessenger(nullptr),
    kDefAlpha(G4Alpha::Definition()),
    kDefOxygen(G4IonTable::GetIonTable()->GetIon(8, 16)),
    kDefGamma(G4Gamma::Definition())
{
    fGenPhaseSpace = new TGenPhaseSpace();
    SetProcessSubType(fCapture);
    DefineCommands();
    ForceReactionByKinE(8.0*MeV);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CarbonAlphaProcess::~CarbonAlphaProcess()
{
    delete fGenPhaseSpace;
    delete fMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool CarbonAlphaProcess::IsApplicable(const G4ParticleDefinition &Def)
{
    return Def.GetParticleName() == "GenericIon";
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VParticleChange *CarbonAlphaProcess::PostStepDoIt(const G4Track &aTrack, const G4Step &)
{
    // initialize particle chagne (G4ParticleChange aParticleChange;G4VParticleChange *pParticleChange = &aParticleChange;)
    aParticleChange.Initialize(aTrack);
    aParticleChange.SetNumberOfSecondaries(2);
    
    // Generate phase space
    // The base unit of ROOT is GeV(GeV/c) and MeV(MeV/c) for Geant4;
    G4double energyUnitCnv = MeV/GeV;

    // generate random value for alpha particle(Not implemented yet)
    G4ThreeVector pAlpha = GenerateMomInTempNR(kDefAlpha);
    G4double p1, p2, p3, p4;
    p1 = aTrack.GetMomentum().getX() + pAlpha.getX();
    p2 = aTrack.GetMomentum().getY() + pAlpha.getY();
    p3 = aTrack.GetMomentum().getZ() + pAlpha.getZ();
    p4 = aTrack.GetTotalEnergy() + kDefAlpha->GetPDGMass();
    TLorentzVector lCarbonAlpha(p1*energyUnitCnv, p2*energyUnitCnv, p3*energyUnitCnv, p4*energyUnitCnv);
    
    // init and generate
    G4double massArr[2] = {kDefOxygen->GetPDGMass()*energyUnitCnv, kDefGamma->GetPDGMass()*energyUnitCnv};
    fGenPhaseSpace->SetDecay(lCarbonAlpha, 2, massArr);
    fGenPhaseSpace->Generate();

    // Get states of daughter particles using TGenPhaseSpace
    TLorentzVector *lv;
    // Oxygen state
    lv = fGenPhaseSpace->GetDecay(0);
    fDynamicOxygen = new G4DynamicParticle(kDefOxygen,
        G4ThreeVector(lv->Px()/energyUnitCnv, lv->Py()/energyUnitCnv, lv->Pz()/energyUnitCnv));
    fDynamicOxygen->SetCharge(fOxygenCharge);

    // gamma state
    lv = fGenPhaseSpace->GetDecay(1);
    fDynamicGamma = new G4DynamicParticle(kDefGamma,
        G4ThreeVector(lv->Px()/energyUnitCnv, lv->Py()/energyUnitCnv, lv->Pz()/energyUnitCnv));
    
    aParticleChange.AddSecondary(fDynamicOxygen);
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

G4double CarbonAlphaProcess::GetMeanFreePath(const G4Track &track, G4double, G4ForceCondition *condition)
{
    // check ions is C12
    if(track.GetDynamicParticle()->GetDefinition()->GetParticleName() != "C12")
    {
        *condition = InActivated;
        return DBL_MAX;
    }
    // if set to force reaction by kinetic energy
    if(fEnergyOfReaction > 0)
        if(track.GetKineticEnergy() < fEnergyOfReaction)
        {
            *condition = Forced;
            return 0;
        }
    // if set to force reaction by track length
    if(fTrackLenOfReaction > 0)
        if(track.GetTrackLength() > fTrackLenOfReaction)
        {
            *condition = Forced;
            return 0;
        }
    // to do : addition of new estimation of mean free path using cross section data.
    *condition = NotForced;
    return DBL_MAX;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreeVector CarbonAlphaProcess::GenerateMomInTempNR(const G4ParticleDefinition *, G4double)
{
    // not yet implemented
    return G4ThreeVector(0., 0., 0.);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CarbonAlphaProcess::ForceReactionByKinE(G4double kinE)
{
    fEnergyOfReaction = kinE;
    fTrackLenOfReaction = -1;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CarbonAlphaProcess::ForceReactionByTrackLen(G4double length)
{
    fEnergyOfReaction = -1;
    fTrackLenOfReaction = length;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CarbonAlphaProcess::DefineCommands()
{
    fMessenger = new G4GenericMessenger(this, "/attpc/process/CarbonAlpha/", "Carbon alpha process control");
    auto verboseCmd = fMessenger->DeclareProperty("verbose", verboseLevel, "Verbosity control of the process");
    verboseCmd.SetParameterName("verbose", false);
    verboseCmd.SetRange("verbose >= 0");
    auto forceByKinEcmd = fMessenger->DeclareMethodWithUnit("forceByKinEnergy", "MeV",
        &CarbonAlphaProcess::ForceReactionByKinE, "The reaction will occur at a given kinetic energy.");
    forceByKinEcmd.SetParameterName("kinE", false);
    forceByKinEcmd.SetRange("kinE > 0");
    auto forceByLengCmd = fMessenger->DeclareMethodWithUnit("forceByTrackLen", "mm",
        &CarbonAlphaProcess::ForceReactionByKinE, "The reaction will occur at a given track length.");
    forceByLengCmd.SetParameterName("tracklen", false);
    forceByLengCmd.SetRange("tracklen > 0");
}