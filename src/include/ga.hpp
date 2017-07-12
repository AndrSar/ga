#pragma once

#ifndef _GA_HPP_
#define _GA_HPP_

#include "random_generator.hpp"
#include "detail/detail.hpp"
#include "genotype_model.hpp"
#include "population.hpp"
#include "functions.hpp"

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


template <class GenotypeModel>
class algorithm
{
public:
    using Genotype = typename GenotypeModel::representation;
    using population_type = population<GenotypeModel>;

    algorithm(const std::shared_ptr<GenotypeModel> model,
              functions::fitness<Genotype> fitness_function):
            model(model),
            fitness_function(fitness_function),
            generations_count(0),
            best_achieved_fitness(0),
            time_passed(0)

    {
    }

    population_type run(const parameters& params)
    {
        population_type pop(model.lock(), params.population_size);
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
    std::weak_ptr<GenotypeModel> model;
    functions::fitness<Genotype> fitness_function;
    std::chrono::milliseconds time_passed;
    unsigned long generations_count;
    double best_achieved_fitness;
};

}

#endif // _GA_HPP_