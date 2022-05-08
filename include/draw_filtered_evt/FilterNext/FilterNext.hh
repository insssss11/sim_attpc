#ifndef FilterNext_h
#define FilterNext_h 1

#include "draw_filtered_evt/Iterator/IIterator.hh"
#include "draw_filtered_evt/Specification/ISpecification.hh"

template <typename T>
class FilterNext
{
    public:
    virtual bool NextFiltered(IIterator<T> &iter, const ISpecification<T> &spec)
    {
        while(true)
        {
            if(!iter.Next())
                return false;
            else
            {
                if(spec.IsSatisfied(*iter))
                {
                    filtered = *iter;
                    return true;
                }
                else
                    continue;                
            }

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

