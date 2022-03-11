/// \file IMeanFreePathEval.hh
/// \brief Definition of the IMeanFreePathEval class

#ifndef IMeanFreePathEval_h
#define IMeanFreePathEval_h 1

#include "G4VProcess.hh"
#include "G4Track.hh"

#include <memory>

/// This class provides interface for evaluation of mean free path to be used in G4VProcess.
class MeanFreePathEval
{
    public:
    virtual G4double Eval(const G4Track *aTrack, G4double previousStepSize, G4ForceCondition *condition) = 0;
    virtual void StartOfTrack() = 0;
    virtual void EndOfTrack() = 0;

    void SetReactionRegion(const G4Region *region);
    G4bool CheckReactionRegion(const G4Track *aTrack) const;
    protected:
    G4double ForcedReactionWithZeroStepLen(G4ForceCondition *condition);
    G4double NoReactionInThisStep(G4ForceCondition *condition);
    protected:
    G4Region const *reactionRegion;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#endif
