#pragma once

#include <string>

namespace ga
{

class logger
{
public:
    logger(): on_number_of_generations_passed(5),
              generations_counter(0)
    {
    }

    void log(const std::string &str)
    {
        ++generations_counter;
        if (generations_counter == on_number_of_generations_passed)
        {
            this->_log(str);
            generations_counter = 0;
        }
    }

    virtual void _log(const std::string &str) = 0;

    std::size_t on_number_of_generations_passed;

private:
    std::size_t generations_counter;
};


} // namespace ga
