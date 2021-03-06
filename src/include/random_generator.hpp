#pragma once

#include <cstddef>
#include <random>
#include <iterator>


namespace ga
{

class random_generator
{
public:
    random_generator()
    {
        generator.seed(random_device());
    }

    template <class Distribution>
    typename Distribution::result_type generate(Distribution d)
    {
        return d(generator);
    }

    template <class T>
    T generate_with_uniform_distribution(const T &min, const T &max)
    {
        return T();
    }

    template <class Cont>
    auto &pick_item(Cont &container)
    {
        std::uniform_int_distribution<std::size_t> d(0, container.size() - 1);
        auto it = container.begin();
        std::advance(it, d(generator));
        return *it;
    }

private:
    std::random_device random_device;
    std::minstd_rand generator;
};


template <>
int random_generator::generate_with_uniform_distribution<int>(const int &min, const int &max)
{
    std::uniform_int_distribution<int> d(min, max);
    return generate(d);
}

template <>
short random_generator::generate_with_uniform_distribution<short>(const short &min, const short &max)
{
    std::uniform_int_distribution<short> d(min, max);
    return generate(d);
}

template <>
double random_generator::generate_with_uniform_distribution<double>(const double &min, const double &max)
{
    std::uniform_real_distribution<double> d(min, max);
    return generate(d);
}


template <class T>
struct select_uniform_distribution_type
{
    using type = void;
};

template <>
struct select_uniform_distribution_type<short>
{
    using type = std::uniform_int_distribution<short>;
};

template <>
struct select_uniform_distribution_type<int>
{
    using type = std::uniform_int_distribution<int>;
};

template <>
struct select_uniform_distribution_type<double>
{
    using type = std::uniform_real_distribution<double>;
};


}
