/// \file ParamContainerTableBuilder.cc
/// \brief Implementation of the ParamContainerTableBuilder class

#include "config/ParamContainerTableBuilder.hh"
#include "config/ParamFileReaderFactory.hh"

ParamContainerTableBuilder::ParamContainerTableBuilder()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ParamContainerTableBuilder::~ParamContainerTableBuilder()
{
    ClearReaders();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ParamContainerTableBuilder *ParamContainerTableBuilder::AddParamContainer(
    const G4String &containerReaderType, const G4String &containerName,
    const G4String &fileName)
{
    auto reader = ParamFileReaderFactory::CreateReaderByType(containerReaderType);
    reader->OpenFile(fileName);
    namesAndReaders.emplace_back(containerName, reader);
    return this;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ParamContainerTable *ParamContainerTableBuilder::Build()
{
    auto table = new ParamContainerTable();
    for(auto p : namesAndReaders)
    {
        auto container = new ParamContainer(p.first);
        p.second->FillContainer(container);
        table->AddContainer(p.first, container);
    }
    ClearReaders();
    return table;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ParamContainerTableBuilder::ClearReaders()
{
    for(auto p : namesAndReaders)
        delete p.second;
    namesAndReaders.clear();
}
