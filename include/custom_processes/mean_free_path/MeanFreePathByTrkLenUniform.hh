/// \file MeanFreePathByTrkLenUniform.hh
/// \brief Definition of the MeanFreePathByTrkLenUniform class

#ifndef MeanFreePathByTrkLenUniform_h
#define MeanFreePathByTrkLenUniform_h 1

#include "custom_processes/mean_free_path/MeanFreePathByTrkLen.hh"

class MeanFreePathByTrkLenUniform : public MeanFreePathByTrkLen
{
    public:
    MeanFreePathByTrkLenUniform();
    virtual void StartOfTrack() override;
    virtual void EndOfTrack() override;

    virtual void SetRange(G4double trkLenMin, G4double trkLenMax);
    private:
    G4double trkLenMin, trkLenMax;
};
#endif
