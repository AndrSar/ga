#include "test.hpp"
#include "../include/ga.hpp"
#include "../include/detail.hpp"

#include <iostream>
#include <numeric>
#include <vector>


namespace ga_test
{

class ConsoleLogger : public test::Logger
{
public:
    void log(const std::string &str) override
    {
        std::cout << str << std::endl;
    }
};


void run()
{
    using namespace test;

    auto ga_suite = create_suite("ga");
    auto ga_operators_suite = create_suite("ga::operators");
    auto ga_detail_suite = create_suite("ga::detail");


    ga_detail_suite->add_case("one_point_crossover()", [](auto &assert) {
        std::vector<int> parent_a(10);
        std::iota(parent_a.begin(), parent_a.end(), 0);

        std::vector<int> parent_b(10);
        std::iota(parent_b.begin(), parent_b.end(), 10);

        auto res = ga::detail::one_point_crossover(parent_a, parent_b, 5);

        assert.equal_sequences("first child", res.first, std::vector<int>{0, 1, 2, 3, 4, 15, 16, 17, 18, 19});
        assert.equal_sequences("second child", res.second, std::vector<int>{10, 11, 12, 13, 14, 5, 6, 7, 8, 9});
    });


    ga_detail_suite->add_case("split_by_groups_and_select()", [](auto &assert) {
        {
            std::vector<int> values(18);
            std::iota(values.begin(), values.end(), 0);

            auto res = ga::detail::split_by_groups_and_select(values, 3, [](std::size_t i) {
                return 0.5;
            });

            assert.equal("size of result for 3 groups from 18 elements", res.size(), 9);
            assert.equal_sequences("3 groups from 18 elements with half selection", res,
                                   std::vector<int>{0, 1, 2, 6, 7, 8, 12, 13, 14});
        }

        {
            std::vector<int> values(10);
            std::iota(values.begin(), values.end(), 0);

            auto res = ga::detail::split_by_groups_and_select(values, 2, [](std::size_t i) {
                return 0.5;
            });

            assert.equal("size of result", res.size(), 4);
            assert.equal_sequences("2 groups from 10 elements with half selection", res,
                                   std::vector<int>{0, 1, 5, 6});
        }
    });

    ConsoleLogger logger;
    execute({ga_suite, ga_operators_suite, ga_detail_suite}, logger);
}

}


int main(int argc, const char * const * argv)
{
    ga_test::run();
    return 0;
}