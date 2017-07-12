#pragma once

#include <cstddef>
#include <functional>


namespace ga
{
namespace functions
{

using rank_distribution = std::function<double(std::size_t)>;

template <class Genotype>
using fitness = std::function<double(const Genotype &)>;

} // functions
} // namespace ga
