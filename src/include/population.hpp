#pragma once

#include "detail/detail.hpp"
#include "genotype_constructor.hpp"
#include "functions.hpp"

#include <cstddef>
#include <vector>
#include <algorithm>
#include <memory>


namespace ga
{

template <class GenotypeModel>
class population
{
public:
    using Genotype = typename GenotypeModel::representation;
    using GenotypeConstructor = genotype_constructor<GenotypeModel>;

    struct genotype_fitness
    {
        double fitness;
        Genotype *genotype;

        genotype_fitness(): fitness(0),
                            genotype(nullptr)
        {
        }

        genotype_fitness(double fitness, Genotype *ptr):
                fitness(fitness),
                genotype(ptr)
        {
        }
    };

public:
    population(const std::shared_ptr<GenotypeModel> &model, std::size_t max_size):
            max_size(max_size),
            model(model),
            constructor(model),
            best_achieved_fitness(0),
            overall_fitness(0)
    {
        generation.reserve(max_size);
        fitness_values.reserve(max_size);
    }


    std::size_t size() const
    {
        return generation.size();
    }


    void init()
    {
        fitness_values = std::vector<genotype_fitness>(max_size);

        for (std::size_t i = 0; i < max_size; ++i)
        {
            generation.push_back(constructor.construct_random());
        }
    }


    void make_selection(const std::size_t ranking_groups_number, functions::rank_distribution func)
    {
        sort_fitness_values();
        std::vector<Genotype> new_generation;
        new_generation.reserve(max_size);

        const auto new_gen_ptrs = detail::split_by_groups_and_select(fitness_values, ranking_groups_number, func);

        for (std::size_t i = 0; i < new_gen_ptrs.size(); ++i)
        {
            new_generation.push_back(std::move(*(new_gen_ptrs[i].genotype)));
            fitness_values[i].fitness = new_gen_ptrs[i].fitness;
            fitness_values[i].genotype = &new_generation.back();
        }

        generation = std::move(new_generation);
    }


    void calculate_fitness(functions::fitness<Genotype> &func)
    {
        double fitness_sum = 0;
        best_achieved_fitness = 0;
        for (std::size_t i = 0; i < generation.size(); ++i)
        {
            const double fitness = func(generation[i]);
            fitness_sum += fitness;
            fitness_values[i] = genotype_fitness(fitness, &generation[i]);
            if (fitness > best_achieved_fitness)
                best_achieved_fitness = fitness;
        }

        overall_fitness = fitness_sum / generation.size();
    }


    void reproduce()
    {
        random_generator rg;

        const std::size_t last_generation_member_index = size() - 1;
        const std::size_t amount = max_size - size();

        std::size_t first_parent = 0;
        for (std::size_t k = 0; k < amount; k += 2)
        {
            const std::size_t second_parent = rg.generate(
                    std::uniform_int_distribution<std::size_t>(first_parent + 1, last_generation_member_index));

            auto children = model->crossover(generation[first_parent], generation[second_parent]);
            model->mutate(children.first);
            model->mutate(children.second);

            generation.push_back(std::move(children.first));
            if (k + 1 < amount) generation.push_back(std::move(children.second));

            ++first_parent;
        }
    }


    void evolve(functions::fitness<Genotype> &fitness_func,
                functions::rank_distribution &rank_func,
                const std::size_t ranking_groups_number)
    {
        calculate_fitness(fitness_func);
        make_selection(ranking_groups_number, rank_func);
        reproduce();
    }


    double get_best_achieved_fitness() const
    {
        return best_achieved_fitness;
    }

    double get_overall_fitness() const
    {
        return overall_fitness;
    }

    GenotypeModel &get_genotype_model()
    {
        return *model;
    }

    const Genotype &get_best_genotype() const
    {
        return *fitness_values.front().genotype;
    }


    void sort_fitness_values()
    {
        std::sort(fitness_values.begin(), fitness_values.end(), [](genotype_fitness &a, genotype_fitness &b) {
            return a.fitness > b.fitness;
        });
    }

private:
    std::shared_ptr<GenotypeModel> model;
    GenotypeConstructor constructor;
    std::size_t max_size;
    std::vector<Genotype> generation;
    std::vector<genotype_fitness> fitness_values;
    double best_achieved_fitness;
    double overall_fitness;
};

} // namespace ga
