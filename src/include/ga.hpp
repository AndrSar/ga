#pragma once

#ifndef _GA_HPP_
#define _GA_HPP_

#include "random_generator.hpp"
#include "detail/detail.hpp"
#include "genotype_model.hpp"
#include "population.hpp"
#include "functions.hpp"
#include "statistics.hpp"
#include "logging/logger.hpp"

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
                  desired_fitness_cap(0.9),
                  time_limit(std::chrono::milliseconds(5000)),
                  ranking_groups_number(5),
                  gather_statistics(false)
    {

    }

    std::size_t population_size;
    std::size_t generations_limit;
    double desired_fitness_cap;
    std::chrono::milliseconds time_limit;
    std::size_t ranking_groups_number;
    bool gather_statistics;
};


template <class GenotypeModel>
class algorithm
{
public:
    using genotype_representation = typename GenotypeModel::representation;
    using population_type = population<GenotypeModel>;
    using fitness_function_type = functions::fitness<genotype_representation>;

public:
    algorithm(const std::shared_ptr<GenotypeModel> &model,
              fitness_function_type fitness_function,
              functions::rank_distribution rank_distribution_function):
            model(model),
            fitness_function(fitness_function),
            rank_distribution_function(rank_distribution_function),
            num_of_generations_passed(0),
            best_achieved_fitness(0),
            time_passed(0)

    {
    }

    population_type run(const parameters& params, logging::logger &log)
    {
        //stats.reserve_generation_stats_space()
        population_type population(model.lock(), params.population_size);
        population.init();

        const auto start_time = std::chrono::steady_clock::now();
        time_passed = std::chrono::milliseconds(0);

        num_of_generations_passed = 0;
        best_achieved_fitness = 0.0;

        while (params.generations_limit > num_of_generations_passed &&
               params.desired_fitness_cap > best_achieved_fitness &&
               params.time_limit > time_passed)
        {
            population.evolve(fitness_function, rank_distribution_function, params.ranking_groups_number);
            best_achieved_fitness = population.get_best_achieved_fitness();

            const auto now = std::chrono::steady_clock::now();
            time_passed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time);
            ++num_of_generations_passed;
        }

        //log._log("Took " + std::to_string(time_passed.count()) + " milliseconds");

        return population;
    }

private:
    std::weak_ptr<GenotypeModel> model;
    fitness_function_type fitness_function;
    functions::rank_distribution rank_distribution_function;
    std::chrono::milliseconds time_passed;
    std::size_t num_of_generations_passed;
    double best_achieved_fitness;
    statistics stats;
};

}

#endif // _GA_HPP_