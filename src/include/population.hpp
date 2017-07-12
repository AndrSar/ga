#pragma once

#include "detail/detail.hpp"
#include <vector>
#include <algorithm>


namespace ga
{

template <class GenotypeModel>
class population
{
public:
    using Genotype = typename GenotypeModel::representation;

    struct genotype_fitness
    {
        double fitness;
        Genotype *genotype;
    };

public:
    population(std::size_t size): max_population_size(size)
    {
        generation.reserve(max_population_size);
        fitness_values.reserve(max_population_size);
    }


    void init()
    {
        generation = std::vector<Genotype>(max_population_size);
        fitness_values = std::vector<genotype_fitness>(max_population_size);

        for (auto &g: generation)
        {
            init_function(g);
        }
    }


    void make_selection(const unsigned int ranking_groups_number)
    {
        sort_fitness_values();
        std::vector<Genotype> new_generation;
        new_generation.reserve(max_population_size);

        const std::size_t basic_group_size = generation.size() / ranking_groups_number;
        std::size_t group_size = basic_group_size;
        for (unsigned int rank_index = 0; rank_index < ranking_groups_number; ++rank_index)
        {
            const std::size_t begin = rank_index * basic_group_size;
            const std::size_t end = begin + group_size;
            for (std::size_t i = begin; i < end; ++i)
            {
                genotype_fitness &gf = fitness_values[i];
                new_generation.push_back(std::move(*gf.genotype));

            }
            group_size = static_cast<std::size_t>(static_cast<float>(group_size) / 1.5f);
        }

        generation = std::move(new_generation);
    }


    void calculate_fitness(std::function<double(const Genotype &)> &fitness_function)
    {
        double fitness_sum = 0;
        max_fitness = 0;
        for (std::size_t i = 0; i < generation.size(); ++i)
        {
            const double fitness = fitness_function(generation[i]);
            fitness_sum += fitness;
            fitness_values[i] = genotype_fitness{fitness, &generation[i]};
            if (fitness > max_fitness)
                max_fitness = fitness;
        }

        overall_fitness = fitness_sum / generation.size();
    }


    void reproduce(std::function<Genotype(const Genotype &, const Genotype &)> crossingover_function)
    {
        random_generator rg;

        const std::size_t last_generation_member_index = generation.size() - 1;

        {
            std::size_t first_parent = 0;
            std::size_t leaders_count_left = max_population_size / 10;

            while (max_population_size > generation.size() && leaders_count_left > 0) {
                std::size_t second_parent =
                        rg.generate(
                std::uniform_int_distribution<unsigned long>(first_parent + 1, last_generation_member_index));
                generation.push_back(crossingover_function(generation[first_parent], generation[second_parent]));
                ++first_parent;
                --leaders_count_left;
            }
        }


        while (max_population_size > generation.size())
        {
            std::size_t first_parent =
            rg.generate(std::uniform_int_distribution<unsigned long>(0, last_generation_member_index));
            std::size_t second_parent =
                    first_parent + 1 < last_generation_member_index ? first_parent + 1 : first_parent - 1;

            generation.push_back(crossingover_function(generation[first_parent], generation[second_parent]));
        }
    }


    double get_max_fitness() const
    {
        return max_fitness;
    }

    double get_overall_fitness() const
    {
        return overall_fitness;
    }

private:
    void sort_fitness_values()
    {
        std::sort(fitness_values.begin(), fitness_values.end(), [](genotype_fitness &a, genotype_fitness &b) {
            return a.fitness > b.fitness;
        });
    }

private:
    std::size_t max_population_size;
    std::vector<Genotype> generation;
    std::vector<genotype_fitness> fitness_values;
    double max_fitness;
    double overall_fitness;
};

} // namespace ga
