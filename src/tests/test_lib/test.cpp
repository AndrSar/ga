//          Copyright Andrey Lifanov 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)


#include "include/test.hpp"

namespace test
{

std::vector<TestSuite::Result> TestSuite::run()
{
    std::vector<Result> results;

    for (auto &c : cases)
    {
        c->operator()();
        for (auto &assertion_result : c->assert.results)
        {
            results.emplace_back(name, c->name, assertion_result);
        }
    }

    return results;
}


auto TestSuite::invalid_results(const std::vector<TestSuite::Result> &results) const
{
    std::vector<Result> v;
    std::copy_if(results.cbegin(), results.cend(), std::back_inserter(v), [](const Result &r){
        return !r.assert_result.is_valid;
    });

    return v;
}


void TestSuite::add_case(const std::string &case_name, LambdaTestCase::Function function)
{
    auto obj = std::make_unique<LambdaTestCase>(case_name, function);
    cases.push_back(std::move(obj));
}


void execute(const std::vector<std::shared_ptr<TestSuite>> &suites, Logger &logger)
{
    std::size_t overall_errors_count = 0;

    for (auto &suite : suites)
    {
        auto invalid_results = suite->invalid_results(suite->run());

        if (!invalid_results.empty())
        {
            overall_errors_count += invalid_results.size();
            logger.log(std::to_string(invalid_results.size()) + " errors in suite '" + suite->get_name() + "':");
            for (auto &result : invalid_results)
            {
                logger.log("-- " + result.case_name + " - " + result.assert_result.check_name + ": " + result.assert_result.error_description);
            }
        }
    }

    if (overall_errors_count > 0)
    {
        logger.log("Total " + std::to_string(overall_errors_count) + " errors occurred.");
    }
    else
    {
        logger.log("Tests are successfully passed.");
    }
}


std::shared_ptr<TestSuite> create_suite(const std::string &name)
{
    return std::make_shared<TestSuite>(name);
}

}