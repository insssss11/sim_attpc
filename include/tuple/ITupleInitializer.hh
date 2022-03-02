/// \file ITupleInitializer.hh
/// \brief Definition of the ITupleInitializer class

#ifndef ITupleInitializer_h
#define ITupleInitializer_h 1

#include "tuple/TupleVectorManager.hh"

#include "AnalysisManager.hh"

#include <string>

/// Interfaces for tuple initializers.
class TupleInitializerBase
{
    public:
    TupleInitializerBase(const std::string &_name = "") : 
    tupleVectorManager(TupleVectorManager::Instance()),
    analysisManager(G4AnalysisManager::Instance()),
    name(_name)
    {}
    virtual void Init() = 0;

    std::string GetName() const { return name; }

    protected:
    TupleVectorManager *tupleVectorManager;
    G4AnalysisManager *analysisManager;

    private:
    const std::string name;
};

class IHitTupleInitializer
{
    public:
    virtual void InitHitTuples() = 0;
};

class IDigiTupleInitializer
{
    public:
    virtual void InitDigiTuples() = 0;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#endif
