
//          Copyright Andrey Lifanov 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#ifndef _GA_API_HPP_
#define _GA_API_HPP_

#include "ga.hpp"

#include <cstddef>
#include <memory>


namespace ga
{
namespace api
{
    template <class T, class FitnessFunc, class RankFunc>
    auto create_algorithm(const std::shared_ptr<genotype_model<T>> &model,
                          FitnessFunc fitness_function,
                          RankFunc rank_function)
    {
        return ga::algorithm<genotype_model<T>>(model, fitness_function, rank_function);
    }


    namespace model
    {
        template <class T>
        using genotype_model_ptr_type = std::shared_ptr<genotype_model<T>>;


        template<class T>
        auto create_homogeneous_model(const T &min_value, const T &max_value, const std::size_t size)
        {
            auto ptr = new genotype_model<T>({min_value, max_value}, size);
            return  std::shared_ptr<genotype_model<T>>(ptr);
        }


        template<class T>
        void set_one_point_crossover(const genotype_model_ptr_type<T> &model)
        {
            model->set_crossover_operator(
                    std::make_unique<ga::operators::one_point_crossover<genotype_model<T>>>()
            );
        }


        template<class T>
        void add_random_value_mutation_with_uniform_distribution(const genotype_model_ptr_type<T> &model, const double probability)
        {
            using distribution_type = typename select_uniform_distribution_type<T>::type;
            model->add_mutation_operator(
                    std::make_unique<ga::operators::random_value_mutation<genotype_model<T>, distribution_type>>(probability)
            );
        }


        template<class T>
        void add_random_value_shift_mutation(const genotype_model_ptr_type<T> &model, const double probability)
        {
            model->add_mutation_operator(
                    std::make_unique<ga::operators::random_value_shift_mutation<genotype_model<T>>>(probability)
            );
        }
    }

} // namespace api
} // namespace ga


#endif //GA_API_HPP
