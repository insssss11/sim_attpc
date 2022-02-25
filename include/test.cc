template<typename vdType>
size_t GetSize(const std::vector<vdType> &data)
{
    std::cout << "Vector type" << std::endl;
    return data.size();
}

template<typename dType>
size_t GetSize(const dType &data)
{
    std::cout << "Normar type" << std::endl;
    return (size_t)99;
}

void test()
{
    std::vector v{1, 2, 3};
    GetSize(v);
    GetSize(1);
}