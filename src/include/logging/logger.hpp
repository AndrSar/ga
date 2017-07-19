#pragma once

#include "../statistics.hpp"
#include <memory>


namespace ga
{
namespace logging
{

class logger
{
public:
    virtual void operator()(const statistics &stat) = 0;
    virtual ~logger() {}
};

} // namespace logging

using list_of_loggers_type = std::vector<std::unique_ptr<logging::logger>>;

} // namespace ga
