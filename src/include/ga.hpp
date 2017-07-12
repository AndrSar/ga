#pragma once

#ifndef _GA_HPP_
#define _GA_HPP_

#include "random_generator.hpp"
#include "detail/detail.hpp"

#include <vector>
#include <cstddef>
#include <chrono>
#include <algorithm>
#include <functional>
#include <memory>
#include <random>
#include <iterator>


namespace ga
{

template <class T>
class genotype
{
public:
    using self = genotype;
    using value_type = typename T::value_type;
    using iterator = typename T::iterator;
    using const_iterator = typename T::const_iterator;

    class mutation_operator;
    class permutation_operator;
    using mutation_operators_list = std::vector<std::unique_ptr<mutation_operator>>;

public:
    class mutation_operator
    {
    public:
        mutation_operator(double probability): probability(probability)
        {
        }

        virtual void apply(genotype &g) = 0;

        double get_probability() const
        {
            return probability;
        }

        void set_probability(const double p)
        {
            probability = p;
        }

    protected:
        double probability;
    };


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





    template <class RandomGenerator>
    void mutate(RandomGenerator &rg, mutation_operators_list &ml, double basic_probability)
    {
        for (auto &m : ml)
        {
            std::bernoulli_distribution bd(basic_probability + m->get_probability());
            if (rg.generate(bd))
            {
                m->apply(*this);
                break;
            }
        }
    }

    void swap_genes(std::size_t index_a, std::size_t index_b)
    {
        iterator a = genes.begin();
        iterator b = a;

        std::advance(a, index_a);
        std::advance(b, index_b);

        std::swap(*a, *b);
    }

    void replace_gene(std::size_t i, const value_type &v)
    {
        iterator it = genes.begin();
        std::advance(it, i);
        *it = v;
    }

    std::size_t size() const
    {
        return genes.size();
    }

    const T &get_genes() const
    {
        return genes;
    }

    T &get_genes()
    {
        return genes;
    }

protected:
    T genes;
};





struct parameters
{
    parameters(): population_size(500),
                  generations_limit(1000000),
                  desired_fitness_cap(0.85),
                  time_limit(std::chrono::milliseconds(5000)),
                  ranking_groups_number(5),
                  genes_mutation_basic_probability(0.01),
                  gather_statistics(false)
    {

    }

    std::size_t population_size;
    unsigned long generations_limit;
    double desired_fitness_cap;
    std::chrono::milliseconds time_limit;
    unsigned int ranking_groups_number;
    double genes_mutation_basic_probability;
    bool gather_statistics;
};


template <class Genotype>
class algorithm
{
public:
    using fitness_function_type = std::function<double(const Genotype &)>;

public:
    algorithm(std::function<void(Genotype&)> &init_function, fitness_function_type &fitness_function):
            fitness_function(fitness_function),
            generations_count(0),
            best_achieved_fitness(0),
            time_passed(0)

    {
    }

    population<Genotype> run(const parameters& params)
    {
        population<Genotype> pop(params.population_size);
        pop.init();

        const auto start_time = std::chrono::steady_clock::now();
        time_passed = std::chrono::milliseconds(0);

        while (params.generations_limit > generations_count &&
               params.desired_fitness_cap > best_achieved_fitness &&
               params.time_limit > time_passed)
        {
            time_passed =
                    std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start_time);
            ++generations_count;
        }

        return pop;
    }

private:
    fitness_function_type fitness_function;
    std::chrono::milliseconds time_passed;
    unsigned long generations_count;
    double best_achieved_fitness;
};

}

#endif // _GA_HPP_