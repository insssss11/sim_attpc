#ifndef EntrySummation_h
#define EntrySummation_h 1

#include <vector>

// Sum up data of all entries.
template <typename dType>
class EntrySummation
{
    public:
    EntrySummation()
    {
        ResetData();
    }
    virtual ~EntrySummation(){}
    
    void ResetData();
    void AddData(const dType &data);
    dType GetData() const;

    private:
    dType data;
};

template <typename dType>
void EntrySummation<dType>::ResetData()
{
    data = 0;
}

template <typename dType>
void EntrySummation<dType>::AddData(const dType &_data)
{
    data += _data;
}

template <typename dType>
dType EntrySummation<dType>::GetData() const
{
    return data;
}

// Specialization for a vector-type template parameter.
template <typename vdType>
class EntrySummation<std::vector<vdType> >
{
    public:
    EntrySummation()
    {
        ResetData();
    }
    virtual ~EntrySummation(){}
    
    void ResetData();
    void AddData(const std::vector<vdType> &data);
    std::vector<vdType> GetData() const;

    private:
    std::vector<vdType> data;
};

template <typename vdType>
void EntrySummation<std::vector<vdType> >::ResetData()
{
    data.clear();
}

template <typename vdType>
void EntrySummation<std::vector<vdType> >::AddData(const std::vector<vdType> &_data)
{
    if(data.empty())
        data.assign(_data.size(), 0);
    for(size_t i = 0;i < _data.size();++i)
        data.at(i) += _data.at(i);
}

template <typename vdType>
std::vector<vdType> EntrySummation<std::vector<vdType> >::GetData() const
{
    return data;
}
#endif

