/// \file PrimaryGeneratorAction.cc
/// \brief Implementation of the PrimaryGeneratorAction class

#include "PrimaryGeneratorAction.hh"
#include "G4SystemOfUnits.hh"
#include "G4IonTable.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction()
    : G4VUserPrimaryGeneratorAction(), fParticleGun(nullptr), fMessenger(nullptr),
    fGunInitialized(false), fPosX(0.*mm), fPosY(-70.*mm), fPosZ(-300*mm)
{
    fParticleGun = new G4ParticleGun();
    DefindCommands();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete fParticleGun;
    delete fMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event *event)
{
    if(!fGunInitialized)
    {
        fParticleGun->SetParticleMomentumDirection(G4ParticleMomentum(0., 0., 1.));
        fParticleGun->SetParticleEnergy(10.*MeV);
        auto pDefinition = G4IonTable::GetIonTable()->GetIon(6, 12);
        fParticleGun->SetParticleDefinition(pDefinition);
        fParticleGun->SetParticleCharge(4.);
        fGunInitialized = true;
    }
    fParticleGun->SetParticlePosition(G4ThreeVector(fPosX, fPosY, fPosZ));
    fParticleGun->GeneratePrimaryVertex(event);
}

//..oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::DefindCommands()
{
    fMessenger = new G4GenericMessenger(this, "/attpc/gun/", "custom gun control");
    fMessenger->DeclarePropertyWithUnit("setPosX", "mm", fPosX, "Set x position");
    fMessenger->DeclarePropertyWithUnit("setPosY", "mm", fPosY, "Set y position");
    fMessenger->DeclarePropertyWithUnit("setPosZ", "mm", fPosZ, "Set z position");
}