/// \file MeanFreePathByTrkLen.hh
/// \brief Definition of the MeanFreePathByTrkLen class

#ifndef MeanFreePathByTrkLen_h
#define MeanFreePathByTrkLen_h 1

#include "custom_processes/mean_free_path/MeanFreePathEval.hh"

class MeanFreePathByTrkLen : public MeanFreePathEval
{
    public:
    MeanFreePathByTrkLen();
    virtual G4double Eval(const G4Track *aTrack, G4double previousStepSize, G4ForceCondition *condition) override;
    virtual void StartOfTrack() override;
    virtual void EndOfTrack() override;

    virtual void SetTrkLen(G4double trkLen);
    protected:
    G4double curTrkLen;
    private:
    G4double trkLen;
};
#endif
