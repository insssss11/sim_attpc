/// \file ITupleInitializer.hh
/// \brief Definition of the ITupleInitializer class

#ifndef ITupleInitializer_h
#define ITupleInitializer_h 1

/// Interfaces for tuple initializers.

class TupleInitializerBase
{
    public:
    virtual void Init() = 0;
};

class IHitTupleInitializer
{
    public:
    virtual void InitHitTuple() = 0;
};

class IDigiTupleInitializer
{
    public:
    virtual void InitDigiTuple() = 0;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#endif
