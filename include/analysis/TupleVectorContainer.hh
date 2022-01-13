/// \file TupleVectorContainer.hh
/// \brief Definition of the TupleVectorContainer class

#ifndef TupleVectorContainer_h
#define TupleVectorContainer_h 1

using namespace std;

#include <unordered_map>
#include <vector>
#include "G4Exception.hh"

/// This class contains vectors whose reference is held by tuples.
template<typename T>
class TupleVectorContainer
{
    public:
    TupleVectorContainer();
    virtual ~TupleVectorContainer();

    vector<T> *GetVectorPtr(const string &tName, const string &vecName) const;

    void AddTuple(const string &tName);

    void AddVector(const string &tName, const string &vecName);
    template<typename str>
    void AddVectors(const string &tName, initializer_list<str> nameList);

    void Reserve(const string &tName, const string &vecName, G4int n);
    void ReserveAll(const string &tName, G4int);
    void Clear(const string &tName, const string &vecName);
    void ClearAll(const string &tName);
    void Reset();

    G4bool ContainTuple(const string &tName) const;
    G4bool ContainVector(const string &tName, const string &vecName) const;

    G4int GetNbOfTuple() const;
    G4int GetNbOfVector(const string &tName) const;

    private:
    void TupleDuplicatedWarning(const string &where, const string &tName) const;
    void TupleNotFoundWarning(const string &where, const string &tName) const;
    void VectorDuplicatedWarning(const string &where, const string &tName, const string &vecName) const;
    void VectorNotFoundWarning(const string &where, const string &tName, const string &vecName) const;

    private:
    unordered_map<string, unordered_map<string, vector<T> > > *fTupleVectorMap;
};

using TupleVectorContainerD = TupleVectorContainer<G4double>;
using TupleVectorContainerI = TupleVectorContainer<G4int>;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename T>
TupleVectorContainer<T>::TupleVectorContainer() : fTupleVectorMap(nullptr)
{
    fTupleVectorMap = new unordered_map<string, unordered_map<string, vector<T> > >;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename T>
TupleVectorContainer<T>::~TupleVectorContainer()
{
    delete fTupleVectorMap;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename T>
vector<T> *TupleVectorContainer<T>::GetVectorPtr(const string &tName, const string &vecName) const
{
    if(!ContainTuple(tName))
    {
        TupleNotFoundWarning("TupleVectorContainer<T>::GetVectorPtr(const string &, const string &)", tName);
        return nullptr;
    }
    else if(!ContainVector(tName, vecName))
    {
        VectorNotFoundWarning("TupleVectorContainer<T>::GetVectorPtr(const string &, const string &)", tName, vecName);
        return nullptr;
    }
    else
        return &fTupleVectorMap->at(tName).at(vecName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename T>
void TupleVectorContainer<T>::AddTuple(const string &tName)
{
    if(ContainTuple(tName))
        TupleDuplicatedWarning("TupleVectorContainer<T>::AddTuple(const string &)", tName);
    else
        fTupleVectorMap->insert(make_pair(tName, unordered_map<string, vector<T> >{}));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename T>
void TupleVectorContainer<T>::AddVector(const string &tName, const string &vecName)
{
    if(!ContainTuple(tName))
        TupleNotFoundWarning("TupleVectorContainer<T>::AddVector(const string &, const string &)", tName);
    else if(ContainVector(tName, vecName))
        VectorDuplicatedWarning("TupleVectorContainer<T>::AddVector(const string &, const string &)", tName, vecName);
    else
        fTupleVectorMap->at(tName).insert(make_pair(vecName, vector<T>{}));
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename T>
template<typename str>
void TupleVectorContainer<T>::AddVectors(const string &tName, initializer_list<str> nameList)
{
    for(auto name : nameList)
        AddVector(tName, name);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename T>
void TupleVectorContainer<T>::Reserve(const string &tName, const string &vecName, G4int n)
{
    if(!ContainTuple(tName))
        TupleNotFoundWarning("TupleVectorContainer<T>::Reserve(const string &, const string &, G4int)", tName);
    else if(!ContainVector(tName, vecName))
        VectorNotFoundWarning("TupleVectorContainer<T>::Reserve(const string &, const string &, G4int)", tName, vecName);
    else
        fTupleVectorMap->at(tName).at(vecName).reserve(n);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename T>
void TupleVectorContainer<T>::ReserveAll(const string &tName, G4int n)
{
    if(!ContainTuple(tName))
        TupleNotFoundWarning("TupleVectorContainer<T>::ReserveAll(const string &, G4int)", tName);
    else
        for(auto &pair : fTupleVectorMap->at(tName))
            pair.second.reserve(n);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename T>
void TupleVectorContainer<T>::Clear(const string &tName, const string &vecName)
{
    if(!ContainTuple(tName))
        TupleNotFoundWarning("TupleVectorContainer<T>::Clear(const string &, const string &)", tName);
    else if(!ContainVector(tName, vecName))
        VectorNotFoundWarning("TupleVectorContainer<T>::Clear(const string &, const string &)", tName, vecName);
    else
        fTupleVectorMap->at(tName).at(vecName).clear();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename T>
void TupleVectorContainer<T>::ClearAll(const string &tName)
{
    if(!ContainTuple(tName))
        TupleNotFoundWarning("TupleVectorContainer<T>::ClearAll(const string &)", tName);
    else
        for(auto &pair : fTupleVectorMap->at(tName))
            pair.second.clear();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename T>
void TupleVectorContainer<T>::Reset()
{
    delete fTupleVectorMap;
    fTupleVectorMap = new unordered_map<string, unordered_map<string, vector<T> > >;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename T>
G4bool TupleVectorContainer<T>::ContainTuple(const string &tName) const
{
    return fTupleVectorMap->find(tName) != fTupleVectorMap->end();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename T>
G4bool TupleVectorContainer<T>::ContainVector(const string &tName, const string &vecName) const
{
    if(!ContainTuple(tName))
    {
        TupleNotFoundWarning("TupleVectorContainer<T>::ContainVector(const string &, const string &)", tName);
        return false;
    }
    else
        return fTupleVectorMap->at(tName).find(vecName) != fTupleVectorMap->at(tName).end();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename T>
G4int TupleVectorContainer<T>::GetNbOfTuple() const
{
    return fTupleVectorMap->size();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename T>
G4int TupleVectorContainer<T>::GetNbOfVector(const string &tName) const
{
    if(!ContainTuple(tName))
    {
        TupleNotFoundWarning("TupleVectorContainer<T>::GetNbOfVector(const string &)", tName);
        return false;
    }
    else
        return fTupleVectorMap->at(tName).size();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename T>
void TupleVectorContainer<T>::TupleDuplicatedWarning(const string &where, const string &tName) const
{
    std::ostringstream message;
    message << "Tuple " << tName << " already exists in the map.";
    G4Exception(where.data(), "TupleVecCon0000", JustWarning, message);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename T>
void TupleVectorContainer<T>::TupleNotFoundWarning(const string &where, const string &tName) const
{
    std::ostringstream message;
    message << "Tuple " << tName << " does not exists in the map.";
    G4Exception(where.data(), "TupleVecCon0001", JustWarning, message);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename T>
void TupleVectorContainer<T>::VectorDuplicatedWarning(const string &where, const string &tName, const string &vecName) const
{
    std::ostringstream message;
    message << "Vector " << vecName << " already exists in the tuple " << tName << ".";
    G4Exception(where.data(), "TupleVecCon0002", JustWarning, message);
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

template<typename T>
void TupleVectorContainer<T>::VectorNotFoundWarning(const string &where, const string &tName, const string &vecName) const
{
    std::ostringstream message;
    message << "Vector " << vecName << " does not exists in the tuple " << tName << ".";
    G4Exception(where.data(), "TupleVecCon0002", JustWarning, message);    
}

#endif
