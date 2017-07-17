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
        std::size_t generation_index;
        double best_achieved_fitness;
    };

public:
    statistics(): best_achieved_fitness(0),
                  milliseconds_passed(0)
    {
    }

    void reserve_generation_stats_space(const std::size_t size)
    {
        generations_stats.reserve(size);
    }

    void create_generation_entry(const std::size_t index, const double fitness)
    {
        generations_stats.push_back({index, fitness});
    }

    void set_best_achieved_fitness(const double value)
    {
        best_achieved_fitness = value;
    }

    void set_milliseconds_passed(const unsigned long value)
    {
        milliseconds_passed = value;
    }

private:
    double best_achieved_fitness;
    unsigned long milliseconds_passed;
    std::vector<generation_record> generations_stats;
};

} // namespace ga
