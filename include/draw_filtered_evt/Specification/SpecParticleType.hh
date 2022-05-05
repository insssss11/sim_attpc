#ifndef SpecParticleType_h
#define SpecParticleType_h 1

#include "draw_filtered_evt/PadEvent.hh"
#include "draw_filtered_evt/Specification/ISpecification.hh"

class SpecParticleType : public ISpecification<PadEvent>
{
    public:
    SpecParticleType(const TrainingDataTypes::EParticle &parEnum) : inverse(false), parEnum{parEnum} 
    {}
    virtual bool IsSatisfied(const PadEvent &event) const override
    {
        for(const auto &parEnum : event.parEnums)
            if(this->parEnum == parEnum)
                return !inverse;
        return inverse;
    }
    void Invert(bool invert = true)
    {
        inverse = invert;
    }
    private:
    bool inverse;
    const TrainingDataTypes::EParticle parEnum;
};

#endif
