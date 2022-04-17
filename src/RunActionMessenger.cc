/// \file RunActionMessenger.cc
/// \brief Definition of the RunActionMessenger class

#include "RunActionMessenger.hh"

#include <string>
#include <vector>

#include "G4Tokenizer.hh"

using namespace std;

RunActionMessenger::RunActionMessenger(RunAction *_runAction)
    : G4UImessenger(), runAction(_runAction)
{
    InitOutputCommands();
}

void RunActionMessenger::InitOutputCommands()
{
    outputDirectory = new G4UIdirectory("/attpc/output/");
    outputDirectory->SetGuidance("Output control");
    
    activateCmd = new G4UIcmdWithABool("/attpc/output/activate", this);
    outputDirectory->SetGuidance("Root output control");
    activateCmd->SetParameterName("active", true);
    activateCmd->SetDefaultValue("true");

    setFileNameCmd = new G4UIcmdWithAString("/attpc/output/setFileName", this);
    setFileNameCmd->SetGuidance("Output file name control");
    setFileNameCmd->SetParameterName("fileName", false);

    G4UIparameter *param;
    mergeFilesCmd = new G4UIcommand("/attpc/output/mergeFiles", this);
    // Only master thead does merging.
    mergeFilesCmd->SetToBeBroadcasted(false);
    mergeFilesCmd->SetGuidance("Manually merge output root files into a single root file");
    param = new G4UIparameter("dstFileName", 's', false);
    mergeFilesCmd->SetParameter(param);
    param = new G4UIparameter("fileName1", 's', false);
    mergeFilesCmd->SetParameter(param);
    param = new G4UIparameter("fileName2", 's', false);
    mergeFilesCmd->SetParameter(param);
    char buffer[256];
    for(int i = 3;i < 9;++i)
    {
        sprintf(buffer, "fileName%d", i);
        param = new G4UIparameter(buffer, 's', true);
        param->SetDefaultValue("");
    }
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunActionMessenger::~RunActionMessenger()
{
    delete outputDirectory;

    delete activateCmd;
    delete setFileNameCmd;
    delete mergeFilesCmd;
}

void RunActionMessenger::SetNewValue(G4UIcommand * command, G4String newValues)
{
    if(command == mergeFilesCmd)
        PassArgsToMergeFilesCmd(newValues);
    else if(command == activateCmd)
        runAction->Activate(activateCmd->ConvertToBool(newValues));
    else if(command == setFileNameCmd)
        runAction->SetFileName(newValues);
}

void RunActionMessenger::PassArgsToMergeFilesCmd(const G4String &newValues)
{
    G4Tokenizer token(newValues);
    string fileName;
    vector<string> subFileNames;
    fileName = token();
    subFileNames.push_back(token());
    subFileNames.push_back(token());
    while(true)
    {
        string name = token();
        if(name.empty())
            break;
        subFileNames.push_back(name);
    }
    runAction->MergeFiles(fileName, subFileNames);
}