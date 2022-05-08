#ifndef PadEvent_h
#define PadEvent_h 1

#include "training_data/TrainingDataTypes.hh"
#include <vector>

struct PadEvent
{
    std::vector<float> qdc;
    std::vector<float> tSig;
    std::vector<int> secFlags;
    // Particle enum and step points of tracks
    std::vector<TrainingDataTypes::EParticle> parEnums;
    std::vector<std::vector<float>> x;
    std::vector<std::vector<float>> y;
};

#endif

