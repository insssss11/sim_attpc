/// \file RunActionMessenger.hh
/// \brief Declaration of the RunActionMessenger class

#ifndef RunActionMessenger_h
#define RunActionMessenger_h 1

#include "RunAction.hh"

#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"

#include "G4UIcommand.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithAString.hh"

class RunAction;

// a messenger class controlling RunAction class.
class RunActionMessenger : public G4UImessenger
{
public:
    RunActionMessenger(RunAction *runAction);
    virtual ~RunActionMessenger();

    void SetNewValue(G4UIcommand * command, G4String newValues);
private:
    void InitOutputCommands();

    void PassArgsToMergeFilesCmd(const G4String &newValues);
private:
    RunAction *runAction;
    
    G4UIdirectory *outputDirectory;
    G4UIcommand *mergeFilesCmd;
    G4UIcmdWithABool *activateCmd;
    G4UIcmdWithAString *setFileNameCmd;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
