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

    protected:
    TupleInitializer();

    private:
    class TupleInitializerFactory{
        public:
        static std::unique_ptr<TupleInitializerBase> Create(
            const std::string &detName);
    };
    virtual void Init() override;

    private:
    static constexpr std::string_view detTuples[] = {"gas_chamber"};
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#endif
