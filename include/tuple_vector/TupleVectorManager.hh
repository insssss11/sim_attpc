/// \file TupleVectorManager.hh
/// \brief Definition of the TupleVectorManager class

#ifndef TupleVectorManager_h
#define TupleVectorManager_h 1

#include "tuple_vector/TupleVectorContainer.hh"

#include "G4Cache.hh"

/// This singleton class manages vectors categorized by tuple names.
class TupleVectorManager
{
    public:
    static TupleVectorManager *Instance();
    void BuildAllTupleVetorContainer();
    virtual ~TupleVectorManager();
    
    TupleVectorContainer *AddTupleVectorContainer(const std::string &tupleName);
    TupleVectorContainer *operator[](const std::string &tupleName);
    TupleVectorContainer *GetTupleVectorContainer(const std::string &tupleName);

    protected:
    TupleVectorManager();
    protected:

    private:
    G4bool ContainerExisits(const std::string &tupleName);

    private:
    // thread-private map containing TupleVectorContainer
    G4MapCache<std::string, TupleVectorContainer> tupleVectorMap;
};
#endif
