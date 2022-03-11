#include "custom_processes/mean_free_path/MeanFreePathByKinEuniform.hh"

#include "Randomize.hh"

MeanFreePathByKinEuniform::MeanFreePathByKinEuniform() : MeanFreePathByKinE(), kinEmin(0), kinEmax(0)
{
}

void MeanFreePathByKinEuniform::StartOfTrack()
{
    SetKinE(CLHEP::RandFlat::shoot(kinEmin, kinEmax));
}

void MeanFreePathByKinEuniform::EndOfTrack()
{
}

void MeanFreePathByKinEuniform::SetRange(G4double _kinEmin, G4double _kinEmax)
{
    kinEmin = _kinEmin;
    kinEmax = _kinEmax;
}
