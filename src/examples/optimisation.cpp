#include "../include/ga.hpp"

#include <vector>
#include <iostream>
#include <memory>


class ConsoleLogger : public ga::Logger
{
public:
    void log(const std::string &str) override
    {
        std::cout << str << std::endl;
    }
};


class optimisation_problem
{
public:
    using genotype_model_type = ga::genotype_model<short>;
    using genotype_representation = genotype_model_type::representation;
    using gene_params_type = genotype_model_type::gene_params;

public:
    optimisation_problem(const std::vector<int> &elements, const int bins_count):
            elements(elements), bins_count(bins_count)
    {
    }

    auto get_genotype_model()
    {
        return std::make_shared<genotype_model_type>(
                gene_params_type{0, static_cast<short>(bins_count - 1)},
                elements.size()
        );
    }

    auto get_solution_quality_function()
    {
        const int sum = std::accumulate(elements.cbegin(), elements.cend(), 0);
        const double expected_bin_load = sum / bins_count;
        return [&elements, expected_bin_load](const genotype_representation &genotype) -> double
        {
            std::vector<int> bins(static_cast<std::size_t>(bins_count));
            for (std::size_t i = 0; i < genotype.size(); ++i)
            {
                bins[static_cast<std::size_t>(genotype[i])] += elements[i];
            }


        };
    }

private:
    std::vector<int> elements;
    int bins_count;
};


int main(int argc, const char * const * argv)
{
    using genotype_model_type = ga::genotype_model<short>;
    using gene_params_type = genotype_model_type::gene_params;

    auto genotype_model = std::make_shared<genotype_model_type>(gene_params_type{0, 2});
    ga::algorithm<genotype_model_type> ga_algorithm{genotype_model, };

    return 0;
}