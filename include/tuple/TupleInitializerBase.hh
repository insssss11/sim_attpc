/// \file TupleInitializerBase.hh
/// \brief Definition of the TupleInitializerBase class

#ifndef TupleInitializerBase_h
#define TupleInitializerBase_h 1

#include "tuple/TupleVectorManager.hh"

#include "AnalysisManager.hh"

#include <string>
#include <unordered_map>
#include <stdexcept>

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

    // [tuple name, [tuple title, activation]]
    static const std::unordered_map<std::string, std::pair<std::string, G4bool > > &GetTuplesActivationStat();

    static void RegisterTupleName(const std::string &name, const std::string &title);

    static void ActivateTuple(const std::string &name, G4bool activate = true);

    protected:
    TupleVectorManager *tupleVectorManager;
    G4AnalysisManager *analysisManager;

    private:
    // [tuple name, [tuple title, activation]]
    static std::unordered_map<std::string, std::pair<std::string, G4bool > > tupleActivated;
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
