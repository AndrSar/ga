
//          Copyright Andrey Lifanov 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <cstddef>
#include <string>
#include <sstream>
#include <vector>


namespace ga
{

class statistics
{
public:
    struct generation_record
    {
        generation_record(const size_t generation_index, const double best_achieved_fitness) :
                generation_index(generation_index),
                best_achieved_fitness(best_achieved_fitness)
        {
        }

        generation_record(): generation_record(0, 0)
        {
        }

        std::size_t generation_index;
        double best_achieved_fitness;
    };

public:
    statistics():
            best_achieved_fitness(0),
            milliseconds_passed(0),
            gather_generations_statistics(false)
    {
    }

    void reserve_generation_stats_space(const std::size_t size)
    {
        if (size > 0)
        {
            gather_generations_statistics = true;
            reserve_generation_stats_space(size);
        }
    }

    void add_generation_stats_entry(const std::size_t index, const double fitness)
    {
        last_generation_stats = generation_record{index, fitness};

        if (gather_generations_statistics)
        {
            generations_stats.push_back(last_generation_stats);
        }
    }

    void set_best_achieved_fitness(const double value)
    {
        best_achieved_fitness = value;
    }

    void set_milliseconds_passed(const long long value)
    {
        milliseconds_passed = value;
    }

    double get_best_achieved_fitness() const
    {
        return best_achieved_fitness;
    }

    long long int get_milliseconds_passed() const
    {
        return milliseconds_passed;
    }

    const generation_record &get_last_generation_stats() const
    {
        return last_generation_stats;
    }

private:
    bool gather_generations_statistics;
    double best_achieved_fitness;
    long long milliseconds_passed;
    generation_record last_generation_stats;
    std::vector<generation_record> generations_stats;
};

} // namespace ga
