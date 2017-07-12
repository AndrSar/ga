#pragma once

#include <cstddef>
#include <functional>


namespace ga
{
namespace detail
{

template<class Cont>
Cont split_by_groups_and_select(
        const Cont &container,
        const std::size_t groups_count,
        std::function<double(std::size_t)> f)
{
    Cont result;
    result.reserve(container.size());
    const std::size_t elements_count_per_group = container.size() / groups_count;

    auto it = container.begin();
    for (std::size_t group_index = 0; group_index < groups_count; ++group_index)
    {
        std::size_t i = 0;
        const std::size_t how_much_to_select_from_group = static_cast<std::size_t>(
                static_cast<double>(elements_count_per_group) * f(group_index));

        while (i < how_much_to_select_from_group)
        {
            result.push_back(std::move(*it));
            ++it;
            ++i;
        }

        std::advance(it, elements_count_per_group - how_much_to_select_from_group);
    }

    return result;
}


template<class Genotype>
auto one_point_crossover(const Genotype &a, const Genotype &b, const std::size_t point_index)
{
    std::pair<Genotype, Genotype> result;

    const std::size_t size = a.size();
    result.first.reserve(size);
    result.second.reserve(size);

    auto it_a = a.cbegin();
    auto it_b = b.cbegin();
    std::advance(it_a, point_index);
    std::advance(it_b, point_index);

    std::copy(a.cbegin(), it_a, std::back_inserter(result.first));
    std::copy(it_b, b.cend(), std::back_inserter(result.first));

    std::copy(b.cbegin(), it_b, std::back_inserter(result.second));
    std::copy(it_a, a.cend(), std::back_inserter(result.second));

    return result;
}

}
}