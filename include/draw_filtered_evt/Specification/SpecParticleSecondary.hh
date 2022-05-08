#ifndef SpecParticleType_h
#define SpecParticleType_h 1

#include "draw_filtered_evt/PadEvent.hh"
#include "draw_filtered_evt/Specification/ISpecification.hh"

class SpecParticleSecondary : public ISpecification<PadEvent>
{
    public:
    SpecParticleSecondary(const TrainingDataTypes::EParticle &parEnum) : parEnum{parEnum} 
    {}
    protected:
    virtual bool IsSatisfiedDoIt(const PadEvent &event) const override
    { 
        return event.secFlags.at(static_cast<size_t>(parEnum)) == 1;
    }
    private:
    bool inverse;
    const TrainingDataTypes::EParticle parEnum;
};

#endif
