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

    }
};


} // namespace logging
} // namespace ga
