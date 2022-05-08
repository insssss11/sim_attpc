#ifndef SpecParticleNum_h
#define SpecParticleNum_h 1

#include "ISpecification.hh"
#include "training_data/TrainingDataTypes.hh"
#include "draw_filtered_evt/PadEvent.hh"

class SpecParticleNum : public ISpecification<PadEvent>
{
    public:
    SpecParticleNum(const int parN) : parN{parN}
    {}
    protected:
    virtual bool IsSatisfiedDoIt(const PadEvent &event) const override
    {
        int parN = 0;
        for(const auto &parEnum : event.parEnums)
            if(parEnum != TrainingDataTypes::Empty && parEnum != TrainingDataTypes::Carbon && parEnum != TrainingDataTypes::Gamma)
                ++parN;
        return this->parN <= parN;
    }
    private:
    const int parN;
};


#endif

