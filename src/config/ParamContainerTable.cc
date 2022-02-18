/// \file ParamContainerTable.cc
/// \brief Implementation of the ParamContainerTable class

#include "config/ParamContainerTable.hh"

#include "G4Exception.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ParamContainerTable *ParamContainerTable::Instance()
{
    static ParamContainerTable *table = new ParamContainerTable();
    return table;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const ParamContainer *ParamContainerTable::GetContainer(const G4String &name)
{
    if(fContainerMap->find(name) == fContainerMap->end())
    {
        std::ostringstream message;
        message << "Container name with " << name << " does not exist!";
        G4Exception("ParamContainerTable::GetContainer(const G4String &name)", "ParamTable0001",
            FatalException, message);
        return nullptr;
    }
    else
        return fContainerMap->at(name);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ParamContainerTable::ParamContainerTable()
{
    fContainerMap = new std::unordered_map<std::string, ParamContainer*>{};
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ParamContainerTable::~ParamContainerTable()
{
    for(auto p : *fContainerMap)
        delete p.second;
    delete fContainerMap;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

std::unique_ptr<ParamContainerTableBuilder> ParamContainerTable::GetBuilder()
{
    return std::make_unique<ParamContainerTableBuilder>();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ParamContainerTable::DumpTable() const
{
    if(!fContainerMap->empty())
    {
        G4cout << "-------------------------------------------------------------" << G4endl;
        G4cout << "# of Parameter Containers : " << fContainerMap->size() << G4endl;
        for(auto container : *fContainerMap)
            container.second->ListParams();
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ParamContainerTable::ClearContainers()
{
    for(auto p : *fContainerMap)
        delete p.second;
    fContainerMap->clear();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ParamContainerTable::AddContainer(std::string name, ParamContainer *container)
{
    if(fContainerMap->find(name) != fContainerMap->end())
    {
        std::ostringstream message;
        message << "Container name with " << name << " is duplicated";
        G4Exception("ParamContainerTable::AddContainer(std::string, ParamContainer *)", "ParamTable0000",
            JustWarning, message);
    }
    else
        fContainerMap->insert(std::make_pair(name, container));
}
