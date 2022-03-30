/// \file ParamContainerTableBuilder.cc
/// \brief Implementation of the ParamContainerTableBuilder class

#include "config/ParamContainerTableBuilder.hh"
#include "config/ParamContainerTableException.hh"
#include "config/ParamFileReaderFactory.hh"

#include <sstream>
#include <fstream>
#include <cctype>

using namespace std;
using namespace ParamContainerTableErrorNum;

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
    try {
        auto reader = ParamFileReaderFactory::CreateReaderByType(containerReaderType);
        reader->OpenFile(fileName);
        namesAndReaders.emplace_back(containerName, reader);
    }
    catch(ParamFileReaderFactoryException const &e)
    {
        e.WarnGeantKernel(JustWarning);
    }
    return this;
}


ParamContainerTable *ParamContainerTableBuilder::BuildFromConfigFile(const G4String &configName)
{
    try{
        ReadConfigureFile(configName);
    }
    catch(ParamContainerTableException const &e)
    {
        e.WarnGeantKernel(FatalException);
    }
    return ParamContainerTableBuilder::Build();
}

void ParamContainerTableBuilder::ReadConfigureFile(const G4String &configName)
{
    ClearReaders();
    ifstream configIn;
    stringstream ss;
    string line, containerReaderType, containerName, fileName;
    configIn.open(configName, ios_base::in);
    if(!configIn.is_open())
        throw ParamContainerTableException("ReadConfigureFile(const G4String &)", CONFIG_FILE_OPEN_FAILURE, configName);
    while(!configIn.eof())
    {
        getline(configIn, line);
        if(line.find_first_of('#') != string::npos)
            line = line.substr(0, line.find_first_of('#'));
        ss.clear();
        ss.str(line);
        ss >> containerReaderType >> containerName >> fileName ;
        if(containerReaderType.empty())
            continue;
        AddParamContainer(containerReaderType, containerName, "parameters/" + fileName);
    }
    configIn.close();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ParamContainerTable *ParamContainerTableBuilder::Build()
{
    auto table = ParamContainerTable::Instance();
    table->ClearContainers();
    for(auto &p : namesAndReaders)
    {
        try
        {
            auto container = make_unique<ParamContainer>(p.first);
            p.second->FillContainer(container.get());
            table->AddContainer(p.first, container.release());
        }
        catch(Exception const &e)
        {
            e.WarnGeantKernel(JustWarning);
        }
    }
    ClearReaders();
    return table;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ParamContainerTableBuilder::ClearReaders()
{
    namesAndReaders.clear();
}
