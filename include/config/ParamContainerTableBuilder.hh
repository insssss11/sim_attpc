/// \file ParamContainerTableBuilder.hh
/// \brief Definition of the ParamContainerTableBuilder class

#ifndef ParamContainerTableBuilder_h
#define ParamContainerTableBuilder_h 1

#include "config/ParamFileReader.hh"
#include "config/ParamContainerTable.hh"
#include "G4String.hh"

#include <vector>

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
    
    // Clear a table if exists and build new one.
    ParamContainerTable *Build();

    private:
    void ClearReaders();

    private:
    // Containers are built in Build() or operator(),
    // using a vector containing pairs of name and reader instances.
    std::vector<std::pair<string, ParamFileReader*> > namesAndReaders;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#endif