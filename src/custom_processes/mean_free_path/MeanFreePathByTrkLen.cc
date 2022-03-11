#include "custom_processes/mean_free_path/MeanFreePathByTrkLen.hh"

MeanFreePathByTrkLen::MeanFreePathByTrkLen() : MeanFreePathEval(), curTrkLen(0.), trkLen(0.)
{
}

G4double MeanFreePathByTrkLen::Eval(const G4Track *aTrack, G4double previousStepSize, G4ForceCondition *condition)
{
    if(!CheckReactionRegion(aTrack))
        return NoReactionInThisStep(condition);
    else
    {
        curTrkLen += previousStepSize;
        if(curTrkLen > trkLen)
            return ForcedReactionWithZeroStepLen(condition);
        else
            return NoReactionInThisStep(condition);
    }
}

void MeanFreePathByTrkLen::StartOfTrack()
{
    curTrkLen = 0;
}

void MeanFreePathByTrkLen::EndOfTrack()
{
}

void MeanFreePathByTrkLen::SetTrkLen(G4double _trkLen)
{
    trkLen = _trkLen;
}
