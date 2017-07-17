#pragma once

#include <cstddef>
#include <string>
#include <sstream>
#include <vector>


namespace ga
{

struct statistics
{
    double best_achieved_fitness;

    struct generation_entry
    {
        std::size_t generation_index;
        double best_achieved_fitness;
    };

    std::vector<generation_entry> generations_stats;
};

} // namespace ga
