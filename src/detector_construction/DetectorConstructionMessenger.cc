/// \file DetectorConstructionMessenger.cc
/// \brief Definition of the DetectorConstructionMessenger class

#include "detector_construction/DetectorConstructionMessenger.hh"
#include "G4Tokenizer.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4RunManager.hh"

#include <string>
#include <vector>

#include "Exception.hh"
using namespace std;

DetectorConstructionMessenger::DetectorConstructionMessenger(DetectorConstruction *detector)
    :G4UImessenger(), fDetector(detector), gasDirectory(nullptr),
    fSetGasCmd(nullptr), fSetMaxStep(nullptr), fSetMaxTrack(nullptr), fSetMaxTime(nullptr), fSetMinKinE(nullptr)
{
    InitGasCommands();
    InitGeometryCommands();
    InitFieldCommands();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstructionMessenger::InitGasCommands()
{
    G4UIparameter *param;

    gasDirectory = new G4UIdirectory("/attpc/gas/");
    gasDirectory->SetGuidance("Gas volume control");

    fSetGasCmd = new G4UIcommand("/attpc/gas/setMixture", this);
    fSetGasCmd->SetGuidance("Set properties of gas mixture.");
    fSetGasCmd->SetGuidance("[usage] /attpc/gas gas1 frac1 [gas2 frac2...]");
    fSetGasCmd->SetGuidance(" gas1:(string) name of gas1");
    fSetGasCmd->SetGuidance(" frac1:(int) fraction of gas1 in percentage");
    fSetGasCmd->SetGuidance(" [gas2 frac2]... : additional gas components");

    param = new G4UIparameter("gas1", 's', false);
    fSetGasCmd->SetParameter(param);

    param = new G4UIparameter("frac1", 'i', true);
    param->SetDefaultValue(100);
    fSetGasCmd->SetParameter(param);
    G4String parName;
    for(int i = 2;i < 7;++i)
    {
        parName = "gas" + std::to_string(i);
        param = new G4UIparameter(parName.c_str(), 's', true);
        param->SetDefaultValue("");
        fSetGasCmd->SetParameter(param);
        parName = "frac" + std::to_string(i);
        param = new G4UIparameter(parName.c_str(), 'i', true);
        param->SetDefaultValue(0);
        fSetGasCmd->SetParameter(param);
    }
    fSetGasCmd->SetRange("frac1 > 0 && frac2 >= 0 && frac3 >= 0 && frac4 >= 0 && frac5 >= 0 && frac6 >= 0");

    printGasStatsCmd = new G4UIcmdWithoutParameter("/attpc/gas/printGasStats", this);
    printGasStatsCmd->SetGuidance("Print information of gas material");

    new G4UnitDefinition("Torr", "Torr", "Pressure", atmosphere/760.);
    setPressureCmd = new G4UIcmdWithADoubleAndUnit("/attpc/gas/setPressure", this);
    setPressureCmd->SetGuidance("Set pressure of gas in the chamber");
    setPressureCmd->SetParameterName("pressure", false);
    setPressureCmd->SetRange("pressure > 0");
    setPressureCmd->SetDefaultUnit("Torr");

    fSetMaxStep = new G4UIcmdWithADoubleAndUnit("/attpc/gas/setStepLimit", this);
    fSetMaxStep->SetGuidance("Set maximum of allowed step length.");
    fSetMaxStep->SetParameterName("ustepMax", false);
    fSetMaxStep->SetDefaultUnit("mm");
    fSetMaxStep->SetRange("ustepMax > 0");

    fSetMaxTrack = new G4UIcmdWithADoubleAndUnit("/attpc/gas/setTrackLimit", this);
    fSetMaxTrack->SetGuidance("Limit length of track.");
    fSetMaxTrack->SetParameterName("utrakMax", false);
    fSetMaxTrack->SetDefaultUnit("mm");
    fSetMaxTrack->SetRange("utrakMax > 0");

    fSetMaxTime = new G4UIcmdWithADoubleAndUnit("/attpc/gas/setTimeLimit", this);
    fSetMaxTime->SetGuidance("Limit global time of track.");
    fSetMaxTime->SetParameterName("utimeMax", false);
    fSetMaxTime->SetDefaultUnit("ns");
    fSetMaxTime->SetRange("utimeMax > 0");

    fSetMinKinE = new G4UIcmdWithADoubleAndUnit("/attpc/gas/setMinKinE", this);
    fSetMinKinE->SetGuidance("Set minimum kinetic remaining.");
    fSetMinKinE->SetParameterName("ukineMin", false);
    fSetMinKinE->SetDefaultUnit("MeV");
    fSetMinKinE->SetRange("ukineMin > 0");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstructionMessenger::InitGeometryCommands()
{
    geometryCmd = new G4UIdirectory("/attpc/geometry/");
    fSetBeamPipePosY = new G4UIcmdWithADoubleAndUnit("/attpc/geometry/setBeamPipePosY", this);
    fSetBeamPipePosY->SetGuidance("Set y-position of beam pipe.");
    fSetBeamPipePosY->SetParameterName("posY", false);
    fSetBeamPipePosY->SetDefaultUnit("mm");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstructionMessenger::InitFieldCommands()
{
    fieldDirectory = new G4UIdirectory("/attpc/field/");
    fieldDirectory->SetGuidance("Field Control");

    setMagneticFieldCmd = new G4UIcmdWith3VectorAndUnit("/attpc/field/bField", this);
    setMagneticFieldCmd->SetParameterName("Bx", "By", "Bz", false);
    setMagneticFieldCmd->SetRange("Bx >= 0 && By >= 0 && Bz >= 0");
    setMagneticFieldCmd->SetDefaultUnit("tesla");

    setElectricFieldCmd = new G4UIcmdWith3Vector("/attpc/field/eField", this);
    setElectricFieldCmd->SetParameterName("Ex", "Ey", "Ez", false);
    setElectricFieldCmd->SetRange("Ex >= 0 && Ey >= 0 && Ez >= 0");
    setElectricFieldCmd->SetGuidance("Electric field control in the unit of V/cm.");
    setElectricFieldCmd->SetGuidance("Warning! No effect on the motion of a charged particle.");
    setElectricFieldCmd->SetGuidance("Only on the electron drift speed and diffusion.");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstructionMessenger::~DetectorConstructionMessenger()
{
    delete gasDirectory;
    delete fSetGasCmd;
    delete printGasStatsCmd;
    delete setPressureCmd;
    delete fSetMaxStep;
    delete fSetMaxTrack;
    delete fSetMaxTime;
    delete fSetMinKinE;

    delete geometryCmd;
    delete fSetBeamPipePosY;

    delete fieldDirectory;
    delete setMagneticFieldCmd;
    delete setElectricFieldCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstructionMessenger::SetNewValue(G4UIcommand *command, G4String newValues)
{
    if(command == fSetGasCmd)
        PassArgsToSetGasComposition(newValues);
    else if(command == printGasStatsCmd)
        fDetector->PrintGasMaterialStats();
    else if(command == setPressureCmd)
        fDetector->SetPressure(setPressureCmd->GetNewDoubleValue(newValues));
    else if(command == fSetMaxStep)
        fDetector->GetUserLimits()->SetMaxAllowedStep(fSetMaxStep->GetNewDoubleValue(newValues));
    else if(command == fSetMaxTrack)
        fDetector->GetUserLimits()->SetUserMaxTrackLength(fSetMaxTrack->GetNewDoubleValue(newValues));
    else if(command == fSetMaxTime)
        fDetector->GetUserLimits()->SetUserMaxTime(fSetMaxTime->GetNewDoubleValue(newValues));
    else if(command == fSetMinKinE)
        fDetector->GetUserLimits()->SetUserMinEkine(fSetMinKinE->GetNewDoubleValue(newValues));
    else if(command == fSetBeamPipePosY)
        fDetector->SetBeamPipePosY(fSetBeamPipePosY->GetNewDoubleValue(newValues));
    else if(command == setMagneticFieldCmd)
        fDetector->SetMagneticField(setMagneticFieldCmd->GetNew3VectorValue(newValues));
    else if(command == setElectricFieldCmd)
        fDetector->SetElectricField(setElectricFieldCmd->GetNew3VectorValue(newValues));
    G4RunManager::GetRunManager()->PhysicsHasBeenModified();
    G4RunManager::GetRunManager()->GeometryHasBeenModified();       
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstructionMessenger::PassArgsToSetGasComposition(const G4String &newValues)
{
    G4Tokenizer token(newValues);
    vector<string> gas;
    vector<double> f;
    string s1, s2;
    while(true)
    {
        s1 = token();
        s2 = token();
        if(s1 == "" || s2 == "0")
            break;
        else
        {
            gas.push_back(s1);
            f.push_back(StoD(s2)/100.);
        }
    }
    try{
        fDetector->SetGasMixture(gas, f);
    }
    catch(Exception const &e)
    {
        e.WarnGeantKernel(JustWarning);
    }
    catch(exception const &e)
    {
        G4cerr << e.what() << G4endl;
    }
}