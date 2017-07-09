#pragma once

#include <cstddef>
#include <random>


namespace ga
{

class random_generator
{
public:
    random_generator(): generator(random_device())
    {
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

    template <>
    int generate_with_uniform_distribution<int>(const int &min, const int &max)
    {
        std::uniform_int_distribution<int> d(min, max);
        return generate(d);
    }

    template <>
    short generate_with_uniform_distribution<short>(const short &min, const short &max)
    {
        std::uniform_int_distribution<short> d(min, max);
        return generate(d);
    }

    template <>
    double generate_with_uniform_distribution<double>(const double &min, const double &max)
    {
        std::uniform_real_distribution<short> d(min, max);
        return generate(d);
    }

private:
    std::random_device random_device;
    std::minstd_rand generator;
};

}
