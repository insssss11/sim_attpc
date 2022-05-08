#ifndef ISpecification_h
#define ISpecification_h 1

template <typename T>
class ISpecification
{
    public:
    ISpecification() : isInverted(false){}

    void Invert(bool invert = true)
    {
        isInverted = invert;
    }

    virtual bool IsSatisfied(const T &item) const
    {
        auto result = IsSatisfiedDoIt(item);
        if(isInverted)
            return !result;
        else
            return result;
    }

    protected:
    virtual bool IsSatisfiedDoIt(const T &item) const = 0;
    
    private:
    bool isInverted;
};

template <typename T>
class AndSpecification : ISpecification<T>
{
    public:
    AndSpecification(ISpecification<T> &first, ISpecification<T> &second) : first{first}, second{second}{}
    protected:
    virtual bool IsSatisfiedDoit(const T &item) const override
    {
        return first.IsSatisfiedDoIt(item) && second.IsSatisfiedDoIt(item);
    }
    private:
    ISpecification<T> &first;
    ISpecification<T> &second;
};

#endif

