/// \file MeanFreePathByKinEuniform.hh
/// \brief Definition of the MeanFreePathByKinEuniform class

#ifndef MeanFreePathByKinEuniform_h
#define MeanFreePathByKinEuniform_h 1

#include "custom_processes/mean_free_path/MeanFreePathByKinE.hh"

class MeanFreePathByKinEuniform : public MeanFreePathByKinE
{
    public:
    MeanFreePathByKinEuniform();
    virtual void StartOfTrack() override;
    virtual void EndOfTrack() override;

    virtual void SetRange(G4double kinEmin, G4double kinEmax);
    private:
    G4double kinEmin, kinEmax;
};
#endif
