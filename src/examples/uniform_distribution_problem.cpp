
//          Copyright Andrey Lifanov 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// This example presents the solution to the problem of even
// placement of objects between given number of bins.

#include "ga.hpp"
#include "logging/console_logger.hpp"

#include <vector>
#include <iostream>
#include <memory>
#include <random>
#include <algorithm>
#include <numeric>


class uniform_distribution_problem
{
public:
    using genotype_model_type = ga::genotype_model<short>;
    using genotype_representation = genotype_model_type::representation;
    using gene_params_type = genotype_model_type::gene_params;

public:
    uniform_distribution_problem(const std::vector<int> &elements, const int bins_count):
            elements(elements), bins_count(bins_count)
    {
    }

    auto construct_genotype_model()
    {
        auto model = std::make_shared<genotype_model_type>(
                gene_params_type{0, static_cast<short>(bins_count - 1)},
                elements.size()
        );

        model->set_crossover_operator(
                std::make_unique<ga::operators::one_point_crossover<genotype_model_type>>()
        );

        using distribution_type = std::uniform_int_distribution<short>;
        model->add_mutation_operator(
                std::make_unique<ga::operators::random_value_mutation<genotype_model_type, distribution_type>>(0.05)
        );

        model->add_mutation_operator(
                std::make_unique<ga::operators::random_value_shift_mutation<genotype_model_type>>(0.05)
        );

        return model;
    }

    auto get_solution_quality_function()
    {
        const int sum = std::accumulate(elements.cbegin(), elements.cend(), 0);
        const double expected_bin_load = sum / bins_count;

        return [this, expected_bin_load](const genotype_representation &genotype) -> double
        {
            std::vector<int> bins(static_cast<std::size_t>(bins_count), 0);
            std::vector<double> load_balance(bins.size(), 0);

            for (std::size_t i = 0; i < genotype.size(); ++i)
            {
                bins[static_cast<std::size_t>(genotype[i])] += elements[i];
            }

            for (std::size_t i = 0; i < bins.size(); ++i)
            {
                const double diff = std::abs(expected_bin_load - static_cast<double>(bins[i]));
                load_balance[i] = 1.0 - (diff / expected_bin_load);
            }

            const double average_result =
                    std::accumulate(load_balance.cbegin(), load_balance.cend(), 0.0) / load_balance.size();
            return average_result;
        };
    }

private:
    std::vector<int> elements;
    int bins_count;
};


int main(int argc, const char * const * argv)
{
    ga::list_of_loggers_type loggers;
    loggers.emplace_back(new ga::logging::console_logger());

    const std::vector<double> vals = {0.5, 1.5, 1.5, 1, 1.5, 1, 1.5, 1, 1.5, 2, 1.5, 2, 2, 1, 2, 0.5, 1, 1.5, 1,
                                      0.5, 2, 2, 1.5, 1, 1.5, 1, 2, 2, 2, 2, 1, 2, 2, 2.5, 1.5, 1.5, 2, 2, 1.5, 2,
                                      2, 1.5, 1.5, 0.6, 0.96, .34, 2, 1.5, 1.5, 1.68, 1.68, 1.34, 1, 1.84, 1.5, 1.84,
                                      1.5, 2, 2, 0.5, 2, 2.5, 1.5, 1.5, 2.34, 2, 1.5, 1, 1.5, 1.34, 2.5, 2.34, 1.5, 2,
                                      1.5, 2.5, 1.5, 2.34, 0.84, 1.5, 1.84, 1.84, 2, 1, 1, 0.84, 2, 0.84, 2, 1.5, 0.84,
                                      2, 2, 1.84, 2};
    std::vector<int> converted_vals;
    for (const auto &val: vals)
    {
        converted_vals.push_back(static_cast<int>(val * 1000.0));
    }

    uniform_distribution_problem problem{converted_vals, 3};

    auto genotype_model =  problem.construct_genotype_model();

    ga::algorithm<uniform_distribution_problem::genotype_model_type> ga_algorithm{
            genotype_model, problem.get_solution_quality_function(), [](std::size_t i) {return 0.5;}};

    ga::parameters params;
    params.time_limit = std::chrono::seconds(60);
    params.desired_fitness_cap = 0.995;
    params.ranking_groups_number = 8;
    //params.population_size = 500;
    auto solution = ga_algorithm.run(params, loggers);

    const auto &genotype = solution.get_best_genotype();

    std::cout << "Best solution: " <<  problem.get_solution_quality_function()(genotype) << std::endl;

    return 0;
}