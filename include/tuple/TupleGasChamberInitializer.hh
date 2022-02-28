/// \file TupleInitilaizer.hh
/// \brief Definition of the TupleInitilaizer class

#ifndef TupleInitilaizer_h
#define TupleInitilaizer_h 1

#include "ITupleInitializer.hh"

/// This class initialize tuples in gas chamber sensitive detector.
class TupleInitializerGasChamber : public TupleInitializerBase, IDigiTupleInitializer, IHitTupleInitializer
{
    public:
    virtual ~TupleInitializerGasChamber();

    protected:
    TupleInitializerGasChamber();

    private:
    virtual void Init() override;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#endif
