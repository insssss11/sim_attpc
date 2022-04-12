/// \file DetectorConstructionMessenger.cc
/// \brief Definition of the DetectorConstructionMessenger class

#include "detector_construction/DetectorConstructionMessenger.hh"
#include "G4Tokenizer.hh"
#include "G4SystemOfUnits.hh"
#include "G4UnitsTable.hh"
#include "G4RunManager.hh"

#include <string>
#include <vector>

#include "exception/Exception.hh"
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
    gasDirectory = new G4UIdirectory("/attpc/gas/");
    gasDirectory->SetGuidance("Gas volume control");

    fSetGasCmd = new G4UIcommand("/attpc/gas/setMixture", this);
    fSetGasCmd->SetGuidance("Set properties of gas mixture.");
    fSetGasCmd->SetGuidance("[usage] /attpc/gas gas1 frac1 gas2 frac2 pressure [temperature]");
    fSetGasCmd->SetGuidance(" gas1          :(string) name of gas1");
    fSetGasCmd->SetGuidance(" frac1         :(int) fraction of gas1 in percentage");
    fSetGasCmd->SetGuidance(" gas1          :(string) name of gas1");
    fSetGasCmd->SetGuidance(" frac1         :(int) fraction of gas1 in percentage");
    fSetGasCmd->SetGuidance(" pressure      : gas pressure in Torr");
    fSetGasCmd->SetGuidance(" [temperature] : temperature in kelvin");
    
    G4UIparameter *param;
    param = new G4UIparameter("gas1", 's', false);
    fSetGasCmd->SetParameter(param);

    param = new G4UIparameter("frac1", 'i', false);
    fSetGasCmd->SetParameter(param);

    param = new G4UIparameter("gas2", 's', false);
    fSetGasCmd->SetParameter(param);

    param = new G4UIparameter("frac2", 'i', false);
    fSetGasCmd->SetParameter(param);

    param = new G4UIparameter("pressure", 'i', false);
    fSetGasCmd->SetParameter(param);

    param = new G4UIparameter("temperature", 'd', true);
    fSetGasCmd->SetParameter(param);

    printGasStatsCmd = new G4UIcmdWithoutParameter("/attpc/gas/printGasStats", this);
    printGasStatsCmd->SetGuidance("Print information of gas material");

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
        fDetector->GetGasMixtureProperties()->DumpProperties();
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
    vector<string> args;
    string arg;
    while(true)
    {
        arg = token();
        if(arg.empty())
            break;
        else
            args.push_back(arg);
    }
    try
    {
        if(!(args.size() == 5 || args.size() == 6))
            throw invalid_argument("Invalid arguments : the number of argmuments must be 5 or 6.");
        else if(args.size() == 5)
            fDetector->SetGasMixture({args[0], args[2]}, {StoI(args[1]), StoI(args[3])}, StoI(args[4]));
        else
            fDetector->SetGasMixture({args[0], args[2]}, {StoI(args[1]), StoI(args[3])}, StoI(args[4]), StoD(args[5])*kelvin);
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