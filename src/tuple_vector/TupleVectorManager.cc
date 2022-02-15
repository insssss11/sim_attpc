/// \file TupleVectorManager.cc
/// \brief Implementation of the TupleVectorManager class

#include "tuple_vector/TupleVectorManager.hh"
#include "AnalysisManager.hh"

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
        throw ContainerDuplicatedWarning("AddTupleVectorContainer(const std::string &)", tupleName);
    else
        tupleVectorMap.Insert(tupleName, TupleVectorContainer(tupleName));
    G4cout << &tupleVectorMap.Get(tupleName) << G4endl;
    return &tupleVectorMap.Get(tupleName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TupleVectorContainer *TupleVectorManager::operator[](const std::string &tupleName)
{
    return GetTupleVectorContainer(tupleName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TupleVectorContainer *TupleVectorManager::GetTupleVectorContainer(const std::string &tupleName)
{
    if(!ContainerExisits(tupleName))
        throw ContainerNotFoundWarning("GetTupleVectorContainer(const std::string &)", tupleName);
    else
        return &tupleVectorMap[tupleName];
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TupleVectorManager::TupleVectorManager()
    : exceptionOriginClass("TupleVectorManager::")
{
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool TupleVectorManager::ContainerExisits(const std::string &tupleName)
{
    return tupleVectorMap.Find(tupleName) != tupleVectorMap.End();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Exception TupleVectorManager::ContainerDuplicatedWarning(
    const std::string &originMethodName, const std::string &tupleName)
{
    std::string message = "TupleVectorConainer with  " + tupleName + " already registered in the manager.";
    return Exception(exceptionOriginClass + originMethodName, "TupleVectorManager0000", JustWarning, message);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Exception TupleVectorManager::ContainerNotFoundWarning(
    const std::string &originMethodName, const std::string &tupleName)
{
    std::string message = "TupleVectorConainer" + tupleName + " does not exists in the manager.";
    return Exception(exceptionOriginClass + originMethodName, "TupleVectorManager0001", JustWarning, message);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
