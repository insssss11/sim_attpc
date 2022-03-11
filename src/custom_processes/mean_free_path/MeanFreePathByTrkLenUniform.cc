#include "custom_processes/mean_free_path/MeanFreePathByTrkLenUniform.hh"

#include "Randomize.hh"

MeanFreePathByTrkLenUniform::MeanFreePathByTrkLenUniform() : MeanFreePathByTrkLen(), trkLenMin(0), trkLenMax(0)
{
}

void MeanFreePathByTrkLenUniform::StartOfTrack()
{
    SetTrkLen(CLHEP::RandFlat::shoot(trkLenMin, trkLenMax));
    curTrkLen = 0;
}

void MeanFreePathByTrkLenUniform::EndOfTrack()
{
}

void MeanFreePathByTrkLenUniform::SetRange(G4double _trkLenMin, G4double _trkLenMax)
{
    trkLenMin = _trkLenMin;
    trkLenMax = _trkLenMax;
}