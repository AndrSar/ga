#pragma once

#include "operators/crossover.hpp"
#include "operators/mutation.hpp"
#include "random_generator.hpp"

#include <vector>
#include <memory>


namespace ga
{

template<class T>
class genotype_model
{
public:
    using self = genotype_model;
    using representation = std::vector<T>;
    using value_type = T;
    using crossover_operator_type = operators::crossover<self>;
    using mutation_operator_type = operators::mutation<self>;

    struct gene_params
    {
        gene_params(const T &min,
                    const T &max,
                    const T &inc,
                    const T &dec,
                    const double p):
                min_value(min),
                max_value(max),
                increment(inc),
                decrement(dec),
                mutation_probability_multiplier(p)
        {
        }

        gene_params(const T &min, const T &max) : gene_params(min, max, 1, 1, 1.0)
        {
        }

        T min_value;
        T max_value;
        T increment;
        T decrement;
        double mutation_probability_multiplier;
    };

public:
    genotype_model(const std::vector <gene_params> &params) : params(params)
    {
    }

    genotype_model(const gene_params &universal, const std::size_t _size)
    {
        params = std::vector<gene_params>{_size, universal};
    }

    gene_params &get_gene_params(const std::size_t index)
    {
        return params[index];
    }

    std::size_t size() const
    {
        return params.size();
    }

    void set_crossover_operator(std::unique_ptr<crossover_operator_type> &&ptr)
    {
        crossover_operator = std::move(ptr);
    }

    void add_mutation_operator(std::unique_ptr<mutation_operator_type> &&ptr)
    {
        mutation_operators.push_back(std::move(ptr));
    }

    void mutate(representation &genotype)
    {
//        for (auto &mut_op : mutation_operators)
//        {
//            std::bernoulli_distribution bd(basic_probability + m->get_probability());
//            if (rg.generate(bd))
//            {
//                m->apply(*this);
//                break;
//            }
//        }

        auto &ptr = rg.pick_item(mutation_operators);
        ptr->apply(*this, genotype);
    }

    auto crossover(const representation &a, const representation &b)
    {
        return crossover_operator->apply(a, b);
    }

private:
    std::vector<gene_params> params;
    std::unique_ptr<crossover_operator_type> crossover_operator;
    std::vector<std::unique_ptr<mutation_operator_type>> mutation_operators;
    random_generator rg;
};

} // namespace ga