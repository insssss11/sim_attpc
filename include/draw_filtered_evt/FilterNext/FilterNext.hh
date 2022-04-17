#ifndef FilterNext_h
#define FilterNext_h 1

#include "draw_filtered_evt/Iterator/IIterator.hh"
#include "draw_filtered_evt/Specification/ISpecification.hh"

template <typename T>
class FilterNext
{
    public:
    virtual bool NextFiltered(IIterator<T> &evtIter, const ISpecification<T> &spec)
    {
        while(true)
        {
            if(!evtIter.Next())
                return false;
            else if(spec.IsSatisfied(*evtIter))
            {
                filtered = *evtIter;
                return true;
            }
            else
                continue;
        }
    }
    const T &GetFiltered()
    {
        return filtered;
    }
    private:
    T filtered;
};

#endif

