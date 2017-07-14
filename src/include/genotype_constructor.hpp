#pragma once

#include "random_generator.hpp"
#include <memory>


namespace ga
{

template <class Model>
class genotype_constructor
{
public:
    using genotype_representation = typename Model::representation;
    using gene_value_type = typename Model::value_type;

public:
    genotype_constructor(const std::shared_ptr<Model> &model): model(model)
    {
    }

    genotype_representation construct_random() const
    {
        random_generator rg;
        genotype_representation result;

        auto _model = model.lock();

        result.reserve(_model->size());

        for (std::size_t i = 0; i < _model->size(); ++i)
        {
            auto gene_params = _model->get_gene_params(i);
            auto value = rg.generate_with_uniform_distribution<gene_value_type>(gene_params.min_value, gene_params.max_value);
            result.push_back(value);
        }

        return result;
    }

private:
    std::weak_ptr<Model> model;
};

} // namespace ga
