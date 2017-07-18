#pragma once

#include "logger.hpp"
#include <iostream>


namespace ga
{
namespace logging
{

class console_logger : public logger
{
public:
    void operator()(const statistics &stat) override
    {
        const auto &generation_statistics = stat.get_last_generation_stats();
        std::cout << generation_statistics.generation_index << " : "
                  << generation_statistics.best_achieved_fitness << std::endl;
    }
};


} // namespace logging
} // namespace ga
