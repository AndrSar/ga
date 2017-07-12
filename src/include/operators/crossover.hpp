#pragma once

#include "../detail/detail.hpp"
#include "../random_generator.hpp"
#include <vector>


namespace  ga
{
namespace operators
{

template <class GenotypeModel>
class crossover
{
public:
    using genotype = typename GenotypeModel::representation;
    using gene_value_type = typename GenotypeModel::value_type;

public:
    virtual std::pair<genotype, genotype> apply(const genotype &a, const genotype &b) = 0;
};


template <class GenotypeModel>
class one_point_crossover : public crossover<GenotypeModel>
{
public:
    std::pair<genotype, genotype>
    apply(const genotype &a, const genotype &b) override
    {
        const std::size_t point_index =
        rg.generate(std::uniform_int_distribution<unsigned long>(1, a.size() - 2));

        return detail::one_point_crossover(a, b, point_index);
    }

private:
    random_generator rg;
};

} //namespace operators
} //namespace ga
