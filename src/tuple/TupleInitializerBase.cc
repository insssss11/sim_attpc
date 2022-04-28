#include "tuple/TupleInitializerBase.hh"

std::unordered_map<std::string, std::pair<std::string, G4bool > > TupleInitializerBase::tupleActivated;

// [tuple name, [tuple title, activation]]
const std::unordered_map<std::string, std::pair<std::string, G4bool > > &TupleInitializerBase::GetTuplesActivationStat()
{
    return tupleActivated;
}

void TupleInitializerBase::RegisterTupleName(const std::string &name, const std::string &title)
{
    tupleActivated[name] = std::make_pair(title, true);
}

void TupleInitializerBase::ActivateTuple(const std::string &name, G4bool activate)
{
    if(tupleActivated.find(name) == tupleActivated.end())
        throw std::invalid_argument("Cannot find tuple named " + name + ". Failed to changed activation.");
    tupleActivated[name].second = activate;
}