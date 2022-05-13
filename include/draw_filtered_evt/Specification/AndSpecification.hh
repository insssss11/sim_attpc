#ifndef AndSpecification_h
#define AndSpecification_h 1

#include "draw_filtered_evt/Specification/ISpecification.hh"

template <typename T>
class AndSpecification : public ISpecification<T>
{
    public:
    AndSpecification(ISpecification<T> &first, ISpecification<T> &second)
        : first{first}, second{second}{}
    protected:
    virtual bool IsSatisfiedDoIt(const T &item) const override
    {
        return first.IsSatisfied(item) && second.IsSatisfied(item);
    }
    private:
    ISpecification<T> &first;
    ISpecification<T> &second;
};
#endif
