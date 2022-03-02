/// \file TupleInitializer.cc
/// \brief Implementation of the TupleInitializer class

#include "tuple/TupleInitializer.hh"
#include "tuple/TupleInitializerGasChamber.hh"

TupleInitializer::TupleInitializer() : TupleInitializerBase(),
    nInitializers(static_cast<int>(nDetectors))
{
    for(int i = 0;i < nInitializers;++i)
        initializers.at(i) = TupleInitializerFactory::Create(i);
}

TupleInitializer::~TupleInitializer()
{
}

std::unique_ptr<TupleInitializerBase> TupleInitializer::TupleInitializerFactory::
    Create(int detectorIdx)
{
    switch(detectorIdx)
    {
        case gas_chamber:
            return std::make_unique<TupleInitializerGasChamber>(TupleInitializerGasChamber{});
        default:
            return nullptr;
    }
}

void TupleInitializer::Init()
{
    for(auto &initializer : initializers)
        initializer->Init();
}