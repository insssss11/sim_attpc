/// \file ParamContainerTable.hh
/// \brief Definition of the ParamContainerTable class

#ifndef ParamContainerTable_h
#define ParamContainerTable_h 1

#include "config/ParamContainer.hh"
#include "config/ParamContainerTableBuilder.hh"

#include "G4String.hh"

#include <string>
#include <memory>
#include <unordered_map>

class ParamContainerTableBuilder;

/// Singleton class prividing access to paramater containers.
class ParamContainerTable
{
    friend ParamContainerTableBuilder;
    public:
    virtual ~ParamContainerTable();

    static const ParamContainer *GetContainer(const G4String &name)
    {
        return fContainerMap->at(name);
    }
    
    static std::unique_ptr<ParamContainerTableBuilder> GetBuilder();
    
    static void DumpTable();
    private:
    ParamContainerTable();
    void AddContainer(std::string name, ParamContainer *container);
    private:
    static ParamContainerTable *fInstance;
    static std::unordered_map<std::string, ParamContainer*> *fContainerMap;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#endif