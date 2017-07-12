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

    virtual void apply(const GenotypeModel &model, genotype &g) = 0;

    double get_probability() const
    {
        return probability;
    }

    void set_probability(const double p)
    {
        probability = p;
    }

protected:
    bool will_apply(const GenotypeModel &model, const std::size_t gene_index)
    {
        double p = probability * model.get_gene_params[gene_index].mutation_probability_multiplier;
        if (p > 1.0) p = 1.0;
        if (p < 0.0) p = 0.0;
        std::bernoulli_distribution bd(p);

        return rg.generate(bd);
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
    random_value_mutation(double probability):
            mutation<GenotypeModel>(probability)
    {
    }

    void apply(const GenotypeModel &model, genotype &g) override final
    {
        const std::size_t index = rg.generate(std::uniform_int_distribution<unsigned long>(0, g.size() - 1));
        if (will_apply(model, index))
        {
            const auto &gene_params = model.get_gene_params[index];
            g[index] = rg.generate(Distribution(gene_params.min_value, gene_params.max_value));
        }
    }
};


template <class GenotypeModel>
class random_value_shift_mutation : public mutation<GenotypeModel>
{
public:
    random_value_shift_mutation(double probability):
            mutation<GenotypeModel>(probability)
    {
    }

    void apply(const GenotypeModel &model, genotype &g) override final
    {
        const std::size_t index = rg.generate(std::uniform_int_distribution<unsigned long>(0, g.size() - 1));
        if (will_apply(model, index))
        {
            const auto &gene_params = model.get_gene_params[index];
            std::bernoulli_distribution bd;
            auto &gene = g[index];
            if (rg.generate(bd))
            {
                gene += gene_params.increment;
            }
            else
            {
                gene -= gene_params.decrement;
            }

            if (gene > gene_params.max_value) gene = gene_params.max_value;
            if (gene < gene_params.min_value) gene = gene_params.min_value;
        }
    }
};

/*
class permutation_operator : public mutation_operator
{
public:
    permutation_operator(double probability, std::size_t max_distance, std::size_t min_distance):
            mutation_operator(probability),
            max_distance(max_distance),
            min_distance(min_distance)
    {

    }

    permutation_operator(): permutation_operator(0, 1, 1)
    {

    }

    void apply(genotype &g) override final
    {
        const std::size_t last_index = g.size() - 1;

        std::size_t first_gene_index = 0;
        std::size_t second_gene_index = rg.generate(std::uniform_int_distribution<unsigned long>(min_distance, max_distance));
        std::size_t index_shift = rg.generate(std::uniform_int_distribution<unsigned long>(0, last_index));

        first_gene_index = index_shift;
        const std::size_t shifted_second_gene_index = second_gene_index + index_shift;
        second_gene_index = shifted_second_gene_index > last_index ?
                            shifted_second_gene_index - last_index :
                            shifted_second_gene_index;

        g.swap_genes(first_gene_index, second_gene_index);
    }

private:
    std::size_t max_distance;
    std::size_t min_distance;
    random_generator rg;
};
*/

} //namespace operators
} //namespace ga