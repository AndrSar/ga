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
        generation_record(): generation_index(0),
                             best_achieved_fitness(0)
        {
        }

        std::size_t generation_index;
        double best_achieved_fitness;
    };

public:
    statistics(const std::size_t generations_stats_reserved_size):
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

private:
    bool gather_generations_statistics;
    double best_achieved_fitness;
    long long milliseconds_passed;
    generation_record last_generation_stats;
    std::vector<generation_record> generations_stats;
};

} // namespace ga
