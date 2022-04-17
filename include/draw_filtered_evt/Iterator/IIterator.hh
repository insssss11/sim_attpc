#ifndef IIterator_h
#define IIterator_h 1

template <typename T>
class IIterator
{
    public:
    virtual bool Next() = 0;
    virtual const T &operator*()
    {
        return item;
    }
    virtual void Reset() = 0;
    protected:
    T item;
};

#endif
