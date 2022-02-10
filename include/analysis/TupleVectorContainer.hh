/// \file TupleVectorContainer.hh
/// \brief Definition of the TupleVectorContainer class

#ifndef TupleVectorContainer_h
#define TupleVectorContainer_h 1

#include "analysis/TupleVectorContainerException.hh"

#include <unordered_map>
#include <vector>
#include <initializer_list>

/// This class contains vectors whose reference is held by tuples.
template<typename T>
class TupleVectorContainer
{
    public:
    TupleVectorContainer();
    virtual ~TupleVectorContainer();

    std::vector<T> *GetVectorPtr(const std::string &tName, const std::string &vecName) const;

    void AddTuple(const std::string &tName);
    void AddVector(const std::string &tName, const std::string &vecName);

    template<typename str>
    void AddVectors(const std::string &tName, std::initializer_list<str> nameList);

    void Clear(const std::string &tName, const std::string &vecName);
    void ClearAll(const std::string &tName);
    void Reset();

    G4bool TupleExists(const std::string &tName) const;
    G4bool VectorExists(const std::string &tName, const std::string &vecName) const;

    G4int GetNbOfTuple() const;
    G4int GetNbOfVector(const std::string &tName) const;

    private:
    std::unordered_map<std::string, std::unordered_map<std::string, std::vector<T> > > *fTupleVectorMap;
    TupleVectorContainerException *exceptionThrower;
};

using TupleVectorContainerD = TupleVectorContainer<G4double>;
using TupleVectorContainerF = TupleVectorContainer<G4float>;
using TupleVectorContainerI = TupleVectorContainer<G4int>;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename T>
TupleVectorContainer<T>::TupleVectorContainer() : fTupleVectorMap(nullptr)
{
    fTupleVectorMap = new std::unordered_map<std::string, std::unordered_map<std::string, std::vector<T> > >;
    exceptionThrower = new TupleVectorContainerException();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename T>
TupleVectorContainer<T>::~TupleVectorContainer()
{
    delete fTupleVectorMap;
    delete exceptionThrower;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename T>
std::vector<T> *TupleVectorContainer<T>::GetVectorPtr(const std::string &tName, const std::string &vecName) const
{
    const std::string where = "GetVectorPtr(const string &, const string &)";
    if(!TupleExists(tName))
    {
        throw exceptionThrower->TupleNotFoundException(where, tName);
        return nullptr;
    }
    else if(!VectorExists(tName, vecName))
    {
        throw exceptionThrower->VectorNotFoundException(where, tName, vecName);
        return nullptr;
    }
    else
        return &fTupleVectorMap->at(tName).at(vecName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename T>
void TupleVectorContainer<T>::AddTuple(const std::string &tName)
{
    const std::string where = "AddTuple(const string &)";
    if(TupleExists(tName))
        throw exceptionThrower->TupleDuplicatedException(where, tName);
    else
        fTupleVectorMap->insert(make_pair(tName, std::unordered_map<std::string, std::vector<T> >{}));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename T>
void TupleVectorContainer<T>::AddVector(const std::string &tName, const std::string &vecName)
{
    const std::string where = "AddVector(const string &, const string &)";
    if(!TupleExists(tName))
        throw exceptionThrower->TupleNotFoundException(where, tName);
    else if(VectorExists(tName, vecName))
        throw exceptionThrower->VectorDuplicatedException(where, tName, vecName);
    else
        fTupleVectorMap->at(tName).insert(make_pair(vecName, std::vector<T>{}));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename T>
template<typename str>
void TupleVectorContainer<T>::AddVectors(const std::string &tName, std::initializer_list<str> nameList)
{
    for(auto name : nameList)
        AddVector(tName, name);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename T>
void TupleVectorContainer<T>::Clear(const std::string &tName, const std::string &vecName)
{
    const std::string where = "Clear(const string &, const string &)";
    
    if(!TupleExists(tName))
        throw exceptionThrower->TupleNotFoundException(where, tName);
    else if(!VectorExists(tName, vecName))
        throw exceptionThrower->VectorNotFoundException(where, tName, vecName);
    
    fTupleVectorMap->at(tName).at(vecName).clear();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename T>
void TupleVectorContainer<T>::ClearAll(const std::string &tName)
{
    const std::string where = "ClearAll(const string &)";
    if(!TupleExists(tName))
        throw exceptionThrower->TupleNotFoundException(where, tName);
    else
        for(auto &pair : fTupleVectorMap->at(tName))
            pair.second.clear();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename T>
void TupleVectorContainer<T>::Reset()
{
    delete fTupleVectorMap;
    fTupleVectorMap = new std::unordered_map<std::string, std::unordered_map<std::string, std::vector<T> > >;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename T>
G4int TupleVectorContainer<T>::GetNbOfTuple() const
{
    return fTupleVectorMap->size();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename T>
G4int TupleVectorContainer<T>::GetNbOfVector(const std::string &tName) const
{
    const std::string where = "GetNbOfVector(const string &)";
    if(!TupleExists(tName))
    {
        throw exceptionThrower->TupleNotFoundException(where, tName);
        return -1;
    }
    else
        return fTupleVectorMap->at(tName).size();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename T>
G4bool TupleVectorContainer<T>::TupleExists(const std::string &tName) const
{
    return fTupleVectorMap->find(tName) != fTupleVectorMap->end();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename T>
G4bool TupleVectorContainer<T>::VectorExists(const std::string &tName, const std::string &vecName) const
{
    return fTupleVectorMap->at(tName).find(vecName) != fTupleVectorMap->at(tName).end();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#endif
