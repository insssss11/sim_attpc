/// \file DetectorConstructionMessenger.cc
/// \brief Definition of the DetectorConstructionMessenger class

#include "DetectorConstructionMessenger.hh"
#include "G4IonTable.hh"
#include "G4Tokenizer.hh"
#include "G4SystemOfUnits.hh"

DetectorConstructionMessenger::DetectorConstructionMessenger(DetectorConstruction *detector)
    :G4UImessenger(), fDetector(detector), fDetectorDirectory(nullptr),
    fSetGasCmd(nullptr)
{
    fDetectorDirectory = new G4UIdirectory("/attpc/gas/");
    fDetectorDirectory->SetGuidance("Gas volume control");

    fSetGasCmd = new G4UIcommand("/attpc/gas/setGas", this);
    fSetGasCmd->SetGuidance("Set properties of gas mixture.");
    fSetGasCmd->SetGuidance("[usage] /attpc/gas gas1 frac1 gas2 frac2 pressure");
    fSetGasCmd->SetGuidance(" gas1:(string) name of gas1");
    fSetGasCmd->SetGuidance(" frac1:(int) fraction of gas1 in percentage");
    fSetGasCmd->SetGuidance(" gas2:(string) name of gas2");
    fSetGasCmd->SetGuidance(" frac2:(int) fraction of gas2 in percentage");
    fSetGasCmd->SetGuidance(" pressure:(double) gas pressure in atm");

    G4UIparameter *param;
    param = new G4UIparameter("gas1", 's', false);
    fSetGasCmd->SetParameter(param);
    param = new G4UIparameter("frac1", 'i', false);
    fSetGasCmd->SetParameter(param);
    param = new G4UIparameter("gas2", 's', false);
    fSetGasCmd->SetParameter(param);
    param = new G4UIparameter("frac2", 'i', false);
    fSetGasCmd->SetParameter(param);
    param = new G4UIparameter("pressure", 'd', true);
    param->SetDefaultValue(1.);
    fSetGasCmd->SetParameter(param);
    fSetGasCmd->SetRange("frac1 > 0 && frac2 > 0 && pressure > 0 && frac1 < 100 && frac2 < 100");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstructionMessenger::~DetectorConstructionMessenger()
{
    delete fDetectorDirectory;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstructionMessenger::SetNewValue(G4UIcommand *command, G4String newValues)
{
    if(command == fSetGasCmd)
        PassArgsToSetGas(newValues);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstructionMessenger::PassArgsToSetGas(const G4String &newValues)
{
    G4Tokenizer token(newValues);
    G4String gas1 = token();
    G4double frac1 = StoD(token());
    G4String gas2 = token();
    G4double frac2 = StoD(token());
    G4double pressure = 1.0;
    G4String sPressure = token();
    if(!sPressure.empty())
        pressure = StoD(sPressure);
    fDetector->SetGas(gas1, frac1, gas2, frac2, pressure*atmosphere);
}