/// \file TupleInitilaizer.hh
/// \brief Definition of the TupleInitilaizer class

#ifndef TupleInitilaizer_h
#define TupleInitilaizer_h 1

#include "ITupleInitializer.hh"
#include "RunAction.hh"

#include "AnalysisManager.hh"
#include <array>
#include <memory>
#include <string_view>

/// This class initialize tuples in every sensitive detectors.
/// Composite pattern adapted.
class TupleInitializer : public TupleInitializerBase
{
    // this class is manipulated only in run action class.
    friend class RunAction;

    public:
    virtual ~TupleInitializer();

    private:
    enum EnumDetectors {gas_chamber, nDetectors};
    TupleInitializer();
    
    virtual void Init() override;

    private:
    class TupleInitializerFactory{
        public:
        static std::unique_ptr<TupleInitializerBase> Create(int detectorIdx);
    };
    const int nInitializers;
    std::array<std::unique_ptr<TupleInitializerBase>, nDetectors> initializers;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#endif
