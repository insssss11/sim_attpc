/// \file TupleInitializer.cc
/// \brief Implementation of the TupleInitializer class

#include "tuple/TupleInitializer.hh"

TupleInitializer::TupleInitializer()
{}

TupleInitializer::~TupleInitializer()
{
}

std::unique_ptr<TupleInitializerBase> TupleInitializer::TupleInitializerFactory::
    Create(const std::string &detName)
{
    if(detName == "gas_chamber")
        return nullptr;
}

void TupleInitializer::Init()
{
    for(auto detTuple : detTuples)
        TupleInitializerFactory::Create(detTuple.data())->Init();
}