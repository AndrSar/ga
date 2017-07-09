#pragma once

#ifndef _GA_HPP_
#define _GA_HPP_

#include "random_generator.hpp"
#include "detail.hpp"

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
class genotype_model
{
public:
    using representation = std::vector<T>;
    using value_type = T;

    struct gene_params
    {
        gene_params(const T& min, const T& max, const double p):
                min_value(min), max_value(max), mutation_probability(p)
        {
        }

        gene_params(const T& min, const T& max): gene_params(min, max, 1.0)
        {
        }

        T min_value;
        T max_value;
        double mutation_probability;
    };

public:
    genotype_model(const std::vector<gene_params> &params): params(params)
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

private:
    std::vector<gene_params> params;
};


template <class Model>
class genotype_constructor
{
public:
    using genotype_representation = typename Model::representation;
    using gene_value_type = typename Model::value_type;

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


namespace operators
{

template <class GenotypeModel>
class mutation
{
public:
    mutation(double probability = 0): probability(probability)
    {
    }

    virtual void apply(typename GenotypeModel::representation &g, GenotypeModel &model) = 0;

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


template <class GenotypeModel>
class crossover
{
public:
    using list = std::vector<std::unique_ptr<crossover>>;

    virtual std::pair<Genotype, Genotype> apply(const Genotype &a, const Genotype &b) = 0;
};


template<class Genotype>
class one_point_crossover : public crossover<Genotype>
{
public:
    std::pair<Genotype, Genotype> apply(const Genotype &a, const Genotype &b) override
    {
        const std::size_t point_index =
                rg.generate(std::uniform_int_distribution<unsigned long>(1, a.size() - 2));

        return detail::one_point_crossover(a, b, point_index);
    }

private:
    random_generator rg;
};



} // namespace operators


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


    template <class Distribution>
    class random_value_mutation_operator : public mutation_operator
    {
    public:
        random_value_mutation_operator(double probability, Distribution d):
                mutation_operator(probability), distribution(d)
        {
        }

        void apply(genotype &g) override final
        {
            std::size_t index = rg.generate(std::uniform_int_distribution<unsigned long>(0, g.size() - 1));
            g.replace_gene(index, rg.generate(distribution));
        }

    private:
        random_generator rg;
        Distribution distribution;
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


template <class Genotype>
class population
{
public:
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