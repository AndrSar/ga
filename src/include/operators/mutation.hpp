#pragma once

#include "../random_generator.hpp"


namespace  ga
{
namespace operators
{

template<class GenotypeModel>
class mutation
{
public:
    using genotype = typename GenotypeModel::representation;
    using gene_value_type = typename GenotypeModel::value_type;

public:
    mutation(double probability = 0) : probability(probability)
    {
    }

    virtual void apply(GenotypeModel &model, genotype &g) = 0;

    double get_probability() const
    {
        return probability;
    }

    void set_probability(const double p)
    {
        probability = p;
    }

protected:
    random_generator rg;

private:
    double probability;
};


template <class GenotypeModel, class Distribution>
class random_value_mutation : public mutation<GenotypeModel>
{
public:
    random_value_mutation(double probability, Distribution d):
            mutation(probability), distribution(d)
    {
    }

    void apply(GenotypeModel &model, genotype &g) override final
    {
        std::size_t index = rg.generate(std::uniform_int_distribution<unsigned long>(0, g.size() - 1));
        g.replace_gene(index, rg.generate(distribution));
    }

private:
    Distribution distribution;
};

} //namespace operators
} //namespace ga