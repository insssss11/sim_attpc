#ifndef ISpecification_h
#define ISpecification_h 1

#include "draw_filtered_evt/PadEvent.hh"

class SpecParticleType : public ISpecification<Event>
{
    public:
    SpecParticleType(const TrainingDataTypes::EParticle &parEnum) : parEnum{parEnum}
    {}
    virtual bool IsSatisfied(const Event &event) const override
    {
        for(const auto &parEnum : event.parEnums)
            if(this->parEnum == parEnum)
                return true;
        return false;
    }
    private:
    const TrainingDataTypes::EParticle parEnum;
};

#endif


