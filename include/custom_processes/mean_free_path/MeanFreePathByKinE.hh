/// \file MeanFreePathByKinE.hh
/// \brief Definition of the MeanFreePathByKinE class

#ifndef MeanFreePathByKinE_h
#define MeanFreePathByKinE_h 1

#include "custom_processes/mean_free_path/MeanFreePathEval.hh"

class MeanFreePathByKinE : public MeanFreePathEval
{
    public:
    MeanFreePathByKinE();
    virtual G4double Eval(const G4Track *aTrack, G4double previousStepSize, G4ForceCondition *condition) override;
    virtual void StartOfTrack() override;
    virtual void EndOfTrack() override;

    virtual void SetKinE(G4double kinE);
    private:
    G4double kinE;
};
#endif
