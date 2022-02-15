/// \file TupleVectorContainer.cc
/// \brief Implementation of the TupleVectorContainer class

#include "tuple_vector/TupleVectorContainer.hh"


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TupleVectorContainer::TupleVectorContainer(const std::string &_containerName)
    : containerName(_containerName), exceptionOriginClass("TupleVectorContainer::")
{
    InitMaps();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TupleVectorContainer::TupleVectorContainer(const TupleVectorContainer &right)
    : TupleVectorContainer(right.GetName())
{
    vectorNameMap = new VectorNameMap(*right.vectorNameMap);
    vectorMapI = new VectorMapI(*right.vectorMapI);
    vectorMapF = new VectorMapF(*right.vectorMapF);
    vectorMapD = new VectorMapD(*right.vectorMapD);
    vectorMapS = new VectorMapS(*right.vectorMapS);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TupleVectorContainer::TupleVectorContainer(TupleVectorContainer &&right)
    : TupleVectorContainer(right.GetName())
{
    vectorNameMap = right.vectorNameMap;
    vectorMapI = right.vectorMapI;
    vectorMapF = right.vectorMapF;
    vectorMapD = right.vectorMapD;
    vectorMapS = right.vectorMapS;
    right.vectorNameMap = nullptr; 
    right.vectorMapI = nullptr; 
    right.vectorMapF = nullptr; 
    right.vectorMapD = nullptr; 
    right.vectorMapS = nullptr; 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TupleVectorContainer::~TupleVectorContainer()
{
    DeleteMaps();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TupleVectorContainer::AddVectorI(const std::string &vecName)
{
    if(VectorExists(vecName))
        throw VectorDuplicatedException("AddVectorI(const std::string &)", vecName);
    else
    {
        vectorNameMap->insert(std::make_pair(vecName, "I"));
        vectorMapI->insert(std::make_pair(vecName, std::vector<G4int>{}));
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TupleVectorContainer::AddVectorF(const std::string &vecName)
{
    if(VectorExists(vecName))
        throw VectorDuplicatedException("AddVectorF(const std::string &)", vecName);
    else
    {
        vectorNameMap->insert(std::make_pair(vecName, "F"));
        vectorMapF->insert(std::make_pair(vecName, std::vector<G4float>{}));
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TupleVectorContainer::AddVectorD(const std::string &vecName)
{
    if(VectorExists(vecName))
        throw VectorDuplicatedException("AddVectorD(const std::string &)", vecName);
    else
    {
        vectorNameMap->insert(std::make_pair(vecName, "D"));
        vectorMapD->insert(std::make_pair(vecName, std::vector<G4double>{}));
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TupleVectorContainer::AddVectorS(const std::string &vecName)
{
    if(VectorExists(vecName))
        throw VectorDuplicatedException("AddVectorS(const std::string &)", vecName);
    else
    {
        vectorNameMap->insert(std::make_pair(vecName, "S"));
        vectorMapS->insert(std::make_pair(vecName, std::vector<G4String>{}));
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

std::vector<G4int> &TupleVectorContainer::GetVectorRefI(const std::string &vecName)
{
    if(!VectorExists(vecName))
        throw VectorNotFoundException("GetVectorRefI(const std::string &)", vecName);
    else
        return vectorMapI->at(vecName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

std::vector<G4float> &TupleVectorContainer::GetVectorRefF(const std::string &vecName)
{
    if(!VectorExists(vecName))
        throw VectorNotFoundException("GetVectorRefF(const std::string &)", vecName);
    else
        return vectorMapF->at(vecName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

std::vector<G4double> &TupleVectorContainer::GetVectorRefD(const std::string &vecName)
{
    if(!VectorExists(vecName))
        throw VectorNotFoundException("GetVectorRefD(const std::string &)", vecName);
    else
        return vectorMapD->at(vecName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

std::vector<G4String> &TupleVectorContainer::GetVectorRefS(const std::string &vecName)
{
    if(!VectorExists(vecName))
        throw VectorNotFoundException("GetVectorRefS(const std::string &)", vecName);
    else
        return vectorMapS->at(vecName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TupleVectorContainer::ClearVectors()
{
    for(auto &vector : *vectorMapI)
        vector.second.clear();
    for(auto &vector : *vectorMapF)
        vector.second.clear();
    for(auto &vector : *vectorMapD)
        vector.second.clear();
    for(auto &vector : *vectorMapS)
        vector.second.clear();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TupleVectorContainer::Reset()
{
    DeleteMaps();
    InitMaps();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4int TupleVectorContainer::GetNbOfVector() const
{
    return vectorNameMap->size();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

const std::string TupleVectorContainer::GetName() const
{
    return containerName;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TupleVectorContainer::FillVectorI(const std::string &vecName, const std::vector<G4int> &vec)
{
    if(!VectorExists(vecName))
        throw VectorNotFoundException(
        "FillVectorI(const std::string &, const std::vector<G4int> &", vecName);
    else
        vectorMapI->at(vecName) = vec;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TupleVectorContainer::FillVectorF(const std::string &vecName, const std::vector<G4float> &vec)
{
    if(!VectorExists(vecName))
        throw VectorNotFoundException(
        "FillVectorF(const std::string &, const std::vector<G4int> &", vecName);
    else
        vectorMapF->at(vecName) = vec;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TupleVectorContainer::FillVectorD(const std::string &vecName, const std::vector<G4double> &vec)
{
    if(!VectorExists(vecName))
        throw VectorNotFoundException(
        "FillVectorD(const std::string &, const std::vector<G4int> &", vecName);
    else
        vectorMapD->at(vecName) = vec;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TupleVectorContainer::FillVectorS(const std::string &vecName, const std::vector<G4String> &vec)
{
    if(!VectorExists(vecName))
        throw VectorNotFoundException(
        "FillVectorS(const std::string &, const std::vector<G4int> &", vecName);
    else
        vectorMapS->at(vecName) = vec;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4bool TupleVectorContainer::VectorExists(const std::string &vecName) const
{
    return vectorNameMap->find(vecName) != vectorNameMap->end();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Exception TupleVectorContainer::VectorNotFoundException(
    const std::string &originMethodName, const std::string &vecName) const
{
    std::string message = "There is no vector with name " + vecName + ".";
    return Exception(exceptionOriginClass + originMethodName, "VectorContainer0000", FatalException, message);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

Exception TupleVectorContainer::VectorDuplicatedException(
    const std::string &originMethodName, const std::string &vecName) const
{
    std::string message = "Vector " + vecName + " already exists in the container " + containerName + ".";
    return Exception(exceptionOriginClass + originMethodName, "VectorContainer0001", JustWarning, message);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TupleVectorContainer::DeleteMaps()
{
    if(vectorNameMap != nullptr)
        delete vectorNameMap;
    if(vectorMapI != nullptr)
        delete vectorMapI;
    if(vectorMapF != nullptr)
        delete vectorMapF;
    if(vectorMapD != nullptr)
        delete vectorMapD;
    if(vectorMapS != nullptr)
        delete vectorMapS;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TupleVectorContainer::InitMaps()
{
    vectorNameMap = new VectorNameMap;
    vectorMapI = new VectorMapI;
    vectorMapF = new VectorMapF;
    vectorMapD = new VectorMapD;
    vectorMapS = new VectorMapS;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......