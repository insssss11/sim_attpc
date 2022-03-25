#include "custom_processes/mean_free_path/MeanFreePathByKinE.hh"

MeanFreePathByKinE::MeanFreePathByKinE() : MeanFreePathEval(), kinE(0.)
{
}

G4double MeanFreePathByKinE::Eval(const G4Track *aTrack, G4double, G4ForceCondition *condition)
{
    if(!CheckReactionRegion(aTrack))
        return NoReactionInThisStep(condition);
    else if(aTrack->GetKineticEnergy() < kinE)
        return ForcedReactionWithZeroStepLen(condition);
    else
        return NoReactionInThisStep(condition);
}

void MeanFreePathByKinE::StartOfTrack()
{
}

void MeanFreePathByKinE::EndOfTrack()
{
}

void MeanFreePathByKinE::SetKinE(G4double _kinE)
{
    kinE = _kinE;
}