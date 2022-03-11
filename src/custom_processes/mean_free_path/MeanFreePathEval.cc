#include "custom_processes/mean_free_path/MeanFreePathEval.hh"

void MeanFreePathEval::SetReactionRegion(const G4Region *region)
{
    reactionRegion = region;
}

G4bool MeanFreePathEval::CheckReactionRegion(const G4Track *aTrack) const
{
    const G4Region *regionOfTrack = aTrack->GetVolume()->GetLogicalVolume()->GetRegion();
    return *reactionRegion == *regionOfTrack;
}

G4double MeanFreePathEval::ForcedReactionWithZeroStepLen(G4ForceCondition *condition)
{
    *condition = Forced;
    return 0.;
}

G4double MeanFreePathEval::NoReactionInThisStep(G4ForceCondition *condition)
{
    *condition = InActivated;
    return DBL_MAX;
}