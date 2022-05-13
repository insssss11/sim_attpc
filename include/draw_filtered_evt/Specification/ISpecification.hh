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
#endif

