#pragma once

#include "random_generator.hpp"


namespace ga
{

template <class Model>
class genotype_constructor
{
public:
    using genotype_representation = typename Model::representation;
    using gene_value_type = typename Model::value_type;

public:
    genotype_constructor(const Model &model): model(model)
    {
    }

    genotype_representation construct_random() const
    {
        random_generator rg;
        genotype_representation result;
        result.reserve(model.size());

        for (std::size_t i = 0; i < model.size(); ++i)
        {
            auto gene_params = model.get_gene_params(i);
            result.push_back(
                    rg.generate_with_uniform_distribution<gene_value_type>(gene_params.min_value, gene_params.max_value)
            );
        }

        return result;
    }

private:
    Model model;
};

} // namespace ga
