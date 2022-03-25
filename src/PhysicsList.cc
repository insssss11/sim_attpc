#include "PhysicsList.hh"
#include "custom_processes/carbon_alpha/CarbonAlphaProcess.hh"

#include "G4RegionStore.hh"
#include "G4ProcessManager.hh"

// step and track limiter process
#include "G4StepLimiter.hh"
#include "G4UserSpecialCuts.hh"

// ionization process and model of generic ions 
#include "G4ionIonisation.hh"
#include "G4BraggIonGasModel.hh"
#include "G4BetheBlochIonGasModel.hh"
#include "G4IonFluctuations.hh"
#include "G4UniversalFluctuation.hh"

// Multipple scattering process and model of generic ions
#include "G4hMultipleScattering.hh"
#include "G4UrbanMscModel.hh"

// Single coulomb scattering process and model of generic ions
#include "G4CoulombScattering.hh"
#include "G4IonCoulombScatteringModel.hh"

// Nuclear stopping process and model of generic ions
#include "G4NuclearStopping.hh"
#include "G4ICRU49NuclearStoppingModel.hh"

#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList::PhysicsList() : G4VModularPhysicsList(),
fMessenger(nullptr)
{
    // protected member of the base class
    verboseLevel = 1;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PhysicsList::~PhysicsList()
{
    delete fMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// particle constructors
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Alpha.hh"
#include "G4Proton.hh"
#include "G4IonConstructor.hh"

void PhysicsList::ConstructParticle()
{
    G4Gamma::Definition();
    G4Electron::Definition();
    G4Positron::Definition();
    G4Proton::Definition();
    G4Alpha::Definition();
    G4IonConstructor ionConstructor;
    ionConstructor.ConstructParticle();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::ConstructProcess()
{
    // transportation
    AddTransportation();
    AddIonGasProcess();
    AddCarbonAlphaProcess();
    AddLimiterProcess();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::EnableIonGasModels(G4bool enable)
{

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::AddIonGasProcess()
{
    auto ph = G4PhysicsListHelper::GetPhysicsListHelper();
    auto pIterator = GetParticleIterator();
    pIterator->reset();
    while((*pIterator)())
    {
        G4ParticleDefinition *pDefinition = pIterator->value();
        G4String pName = pDefinition->GetParticleName();
        if(pName == "proton" || pName == "He3" || pName == "alpha" || pName == "GenericIon")
        {
            // effective charge and energy loss model of ion
            G4ionIonisation *iIon = new G4ionIonisation();
            G4BraggIonGasModel *bIgm = new G4BraggIonGasModel();
            G4BetheBlochIonGasModel *bbIgm = new G4BetheBlochIonGasModel();
            bIgm->SetActivationHighEnergyLimit(2.*MeV*pDefinition->GetPDGMass()/proton_mass_c2);
            bbIgm->SetActivationLowEnergyLimit(2.*MeV*pDefinition->GetPDGMass()/proton_mass_c2);

            iIon->AddEmModel(0, bIgm, new G4IonFluctuations);
            iIon->AddEmModel(0, bbIgm, new G4UniversalFluctuation);
            // no delta ray
            iIon->ActivateSecondaryBiasing("World", 1e-10, 100*TeV);

            G4hMultipleScattering *hMsc = new G4hMultipleScattering();
            hMsc->AddEmModel(0, new G4UrbanMscModel());
            G4CoulombScattering *csc = new G4CoulombScattering();
            csc->AddEmModel(0, new G4IonCoulombScatteringModel());
            G4NuclearStopping *nsp = new G4NuclearStopping();
            nsp->AddEmModel(0, new G4ICRU49NuclearStoppingModel());
            ph->RegisterProcess(iIon, pDefinition);
            ph->RegisterProcess(hMsc, pDefinition);
            ph->RegisterProcess(csc, pDefinition);
            ph->RegisterProcess(nsp, pDefinition);
        }
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::AddCarbonAlphaProcess()
{
    auto ph = G4PhysicsListHelper::GetPhysicsListHelper();
    auto pIterator = GetParticleIterator();
    pIterator->reset();
    while((*pIterator)())
    {
        G4ParticleDefinition *pDefinition = pIterator->value();
        G4String pName = pDefinition->GetParticleName();
        if(pName == "GenericIon")
        {
            auto reactionRegion = G4RegionStore::GetInstance()->GetRegion("gas_chamber");
            CarbonAlphaProcess *cap = new CarbonAlphaProcess();
            cap->SetReactionRegion(reactionRegion);
            ph->RegisterProcess(cap, pDefinition);
        }
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::AddLimiterProcess()
{
    auto ph = G4PhysicsListHelper::GetPhysicsListHelper();
    auto pIterator = GetParticleIterator();
    pIterator->reset();
    G4StepLimiter *slt = new G4StepLimiter();
    G4UserSpecialCuts *usc = new G4UserSpecialCuts();
    while((*pIterator)())
    {
        G4ParticleDefinition *pDefinition = pIterator->value();
        G4String pName = pDefinition->GetParticleName();
        if(pName == "proton" || pName == "He3" || pName == "alpha" || pName == "GenericIon")
        {
            ph->RegisterProcess(slt, pDefinition);
            ph->RegisterProcess(usc, pDefinition);
        }
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PhysicsList::DefineCommands()
{
    fMessenger = new G4GenericMessenger(this, "/attpc/physics/", "...Physics control...");
}