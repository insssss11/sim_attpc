#ifndef SpecTrue_h
#define SpecTrue_h 1

#include "draw_filtered_evt/Specification/ISpecification.hh"

template <typename T>
class SpecTrue : public ISpecification<T>
{
    protected:
    virtual bool IsSatisfiedDoIt(const T &) const override
    {
        return true;
    }
};
#endif

