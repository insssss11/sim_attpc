#ifndef ISpecification_h
#define ISpecification_h 1

template <typename T>
class ISpecification
{
    public:
    virtual bool IsSatisfied(const T &item) const = 0;
};

template <typename T>
class AndSpecification : ISpecification<T>
{
    public:
    AndSpecification(ISpecification<T> &first, ISpecification<T> &second) : first{first}, second{second}{}
    virtual bool IsSatisfied(const T &item) const override
    {
        return first.IsSatisfied(item) && second.IsSatisfied(item);
    }
    private:
    ISpecification<T> &first;
    ISpecification<T> &second;
};

#endif

