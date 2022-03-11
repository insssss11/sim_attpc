/// \file CarbonAlphaProcessMessenger.cc
/// \brief Definition of the CarbonAlphaProcessMessenger class

#include "custom_processes/carbon_alpha/CarbonAlphaProcessMessenger.hh"
#include "G4Tokenizer.hh"
#include "G4SystemOfUnits.hh"

CarbonAlphaProcessMessenger::CarbonAlphaProcessMessenger(CarbonAlphaProcess *_carbonAlpha)
    :G4UImessenger(), carbonAlpha(_carbonAlpha)
{
    carbonAlphaDirectory = new G4UIdirectory("/attpc/process/CarbonAlpha/");
    carbonAlphaDirectory->SetGuidance("carbon-alpha capture process control");

    activate = new G4UIcmdWithABool("/attpc/process/CarbonAlpha/activate", this);
    activate->SetGuidance("Activate/inactivate this process.");
    activate->SetParameterName("activated", true);
    activate->SetDefaultValue(true);

    forceAtKinEcmd = new G4UIcmdWithADoubleAndUnit("/attpc/process/CarbonAlpha/forceAtKinE", this);
    forceAtKinEcmd->SetGuidance("The reaction will occur at a given kinetic energy.");
    forceAtKinEcmd->SetParameterName("kinE", false);
    forceAtKinEcmd->SetDefaultUnit("MeV");
    forceAtKinEcmd->SetRange("kinE > 0");

    forceAtTrkLenCmd = new G4UIcmdWithADoubleAndUnit("/attpc/process/CarbonAlpha/forceAtTrkLen", this);
    forceAtTrkLenCmd->SetGuidance("The reaction will occur at a given track length.");
    forceAtTrkLenCmd->SetParameterName("trkLen", false);
    forceAtTrkLenCmd->SetDefaultUnit("mm");
    forceAtTrkLenCmd->SetRange("trkLen > 0");

    forceAtRndmTrkLenUniform = new G4UIcommand("/attpc/process/CarbonAlpha/forceAtRndmTrkLenUniform", this);
    forceAtRndmTrkLenUniform->SetGuidance("The reaction will occur at a given track length following uniform distributions.");
    forceAtRndmTrkLenUniform->SetGuidance("[usage] /attpc/process/CarbonAlpha/forceAtRndmTrkLenUniform trkLenMin trkLenMax");
    forceAtRndmTrkLenUniform->SetGuidance(" trkLenMin : (double) lower limit of the distribution in mm");
    forceAtRndmTrkLenUniform->SetGuidance(" trkLenMax : (double) upper limit of the distribution in mm");

    G4UIparameter *param;
    param = new G4UIparameter("trkLenMin", 'd', false);
    forceAtRndmTrkLenUniform->SetParameter(param);
    param = new G4UIparameter("trkLenMax", 'd', false);
    forceAtRndmTrkLenUniform->SetParameter(param);
    forceAtRndmTrkLenUniform->SetRange("trkLenMin < trkLenMax && 0 <= trkLenMin");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

CarbonAlphaProcessMessenger::~CarbonAlphaProcessMessenger()
{
    delete carbonAlphaDirectory;
    delete forceAtKinEcmd;
    delete forceAtTrkLenCmd;
    delete forceAtRndmTrkLenUniform;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CarbonAlphaProcessMessenger::SetNewValue(G4UIcommand *command, G4String newValues)
{
    if(command == activate)
        carbonAlpha->Activate(activate->GetNewBoolValue(newValues));
    if(command == forceAtKinEcmd)
        carbonAlpha->ForceAtKinE(forceAtKinEcmd->GetNewDoubleValue(newValues));
    else if(command == forceAtTrkLenCmd)
        carbonAlpha->ForceAtTrkLen(forceAtTrkLenCmd->GetNewDoubleValue(newValues));
    else if(command == forceAtRndmTrkLenUniform)
        PassArgsToForceAtRndmTrkLenUniform(newValues);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void CarbonAlphaProcessMessenger::PassArgsToForceAtRndmTrkLenUniform(const G4String &newValues)
{
    G4Tokenizer token(newValues);
    G4double trkLenMin, trkLenMax;
    trkLenMin = StoD(token());
    trkLenMax = StoD(token());
    carbonAlpha->ForceAtRndmTrkLenUniform(trkLenMin, trkLenMax);
}