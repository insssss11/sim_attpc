/// \file TupleVectorManager.cc
/// \brief Implementation of the TupleVectorManager class

#include "tuple/TupleVectorManager.hh"
#include "tuple/TupleVectorManagerException.hh"
#include "AnalysisManager.hh"

using namespace TupleVectorManagerErrorNum;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TupleVectorManager *TupleVectorManager::Instance()
{
    static TupleVectorManager *instance = new TupleVectorManager();
    return instance;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TupleVectorManager::~TupleVectorManager()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TupleVectorContainer *TupleVectorManager::AddTupleVectorContainer(const std::string &tupleName)
{
    if(ContainerExisits(tupleName))
        throw TupleVectorManagerException("AddTupleVectorContainer(const std::string &)", TUPLE_DUPLICATED, tupleName);
    else
        tupleVectorMap.Insert(tupleName, TupleVectorContainer(tupleName));
    return &tupleVectorMap.Get(tupleName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TupleVectorContainer *TupleVectorManager::GetTupleVectorContainer(const std::string &tupleName)
{
    if(!ContainerExisits(tupleName))
        throw TupleVectorManagerException("GetTupleVectorContainer(const std::string &)", TUPLE_DUPLICATED, tupleName);
    else
        return &tupleVectorMap[tupleName];
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TupleVectorContainer *TupleVectorManager::operator[](const std::string &tupleName)
{
    return GetTupleVectorContainer(tupleName);
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TupleVectorManager::TupleVectorManager()
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool TupleVectorManager::ContainerExisits(const std::string &tupleName)
{
    return tupleVectorMap.Find(tupleName) != tupleVectorMap.End();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
