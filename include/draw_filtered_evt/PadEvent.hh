#ifndef PadEvent_h
#define PadEvent_h 1

#include "training_data/TrainingDataTypes.hh"
#include <vector>

struct PadEvent
{
    std::vector<float> qdc;
    std::vector<std::vector<float>> x;
    std::vector<std::vector<float>> y;
    std::vector<TrainingDataTypes::EParticle> parEnums;
};

#endif

