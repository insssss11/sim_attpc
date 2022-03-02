/// \file TupleInitializerGasChamber.hh
/// \brief Definition of the TupleInitializerGasChamber class

#ifndef TupleInitializerGasChamber_h
#define TupleInitializerGasChamber_h 1

#include "tuple/ITupleInitializer.hh"
#include "tuple/TupleInitializer.hh"

/// This class initialize tuples in gas chamber sensitive detector.
class TupleInitializerGasChamber : public TupleInitializerBase, IDigiTupleInitializer, IHitTupleInitializer
{
    friend class TupleInitializerFactory;
    friend class TupleInitializer;

    public:
    virtual ~TupleInitializerGasChamber();

    protected:
    TupleInitializerGasChamber();

    private:
    virtual void Init() override;
    virtual void InitHitTuples() override;
    virtual void InitDigiTuples() override;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#endif
