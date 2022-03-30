/// \file ParamContainerTableBuilder.hh
/// \brief Definition of the ParamContainerTableBuilder class

#ifndef ParamContainerTableBuilder_h
#define ParamContainerTableBuilder_h 1

#include "config/ParamFileReader.hh"
#include "config/ParamContainerTable.hh"
#include "G4String.hh"

#include <vector>
#include <memory>
class ParamContainerTable;

/// Builder of ParamContainerTable class.
class ParamContainerTableBuilder
{
    public:
    ParamContainerTableBuilder();
    virtual ~ParamContainerTableBuilder();

    ParamContainerTableBuilder *AddParamContainer(
        const G4String &containerReaderType, const G4String &containerName,
        const G4String &fileName);

    ParamContainerTable *BuildFromConfigFile(const G4String &configName);
    // Clear a table if exists and build new one.
    ParamContainerTable *Build();
    private:
    void ReadConfigureFile(const G4String &configName);
    void ClearReaders();

    private:
    // Containers are built in Build() or operator(),
    // using a vector containing pairs of name and reader instances.
    std::vector<std::pair<string, std::unique_ptr<ParamFileReader> > > namesAndReaders;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#endif