/// \file CarbonAlphaProcessMessenger.hh
/// \brief Declaration of the CarbonAlphaProcessMessenger class

#ifndef CarbonAlphaProcessMessenger_h
#define CarbonAlphaProcessMessenger_h 1

#include "custom_processes/carbon_alpha/CarbonAlphaProcess.hh"

#include "G4UImessenger.hh"
#include "G4UIdirectory.hh"

#include "G4UIcommand.hh"
#include "G4UIcmdWithAnInteger.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"
#include "G4UIcmdWithABool.hh"

#include <memory>

class CarbonAlphaProcess;

// a messenger class controlling CarbonAlphaProcess class.
class CarbonAlphaProcessMessenger : public G4UImessenger
{
public:
    CarbonAlphaProcessMessenger(CarbonAlphaProcess *carbonAlpha);
    virtual ~CarbonAlphaProcessMessenger();

    void SetNewValue(G4UIcommand * command, G4String newValues);
private:
    void PassArgsToForceAtRndmTrkLenUniform(const G4String &newValues);

private:
    CarbonAlphaProcess *carbonAlpha;
    G4UIdirectory *carbonAlphaDirectory;
    G4UIcmdWithABool *activate;
    G4UIcmdWithADoubleAndUnit *forceAtKinEcmd;
    G4UIcmdWithADoubleAndUnit *forceAtTrkLenCmd;
    G4UIcommand *forceAtRndmTrkLenUniform;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
