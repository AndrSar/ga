//          Copyright Andrey Lifanov 2017.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <string>
#include <algorithm>


namespace test
{

class TestSuite
{
public:
    struct Assert
    {
        struct Result
        {
            std::string check_name;
            std::string error_description;
            bool is_valid;

            Result(const std::string &name): check_name(name),
                                             is_valid(true)
            {
            }

            void set_invalid(const std::string &text)
            {
                is_valid = false;
                error_description = text;
            }
        };

        void operator()(const std::string &check_name, const bool result)
        {
            Result res{check_name};
            if (!result)
            {
                res.set_invalid("Is not true");
            }

            results.push_back(std::move(res));
        }

        template <class A, class B>
        void equal(const std::string &check_name, const A &a, const B &b)
        {
            Result res{check_name};
            if (a != b)
            {
                res.set_invalid(std::to_string(a) + std::string(" is not equal to ") + std::to_string(b));
            }

            results.push_back(std::move(res));
        }

        template <class A, class B>
        void not_equal(const std::string &check_name, const A &a, const B &b)
        {
            Result res{check_name};
            if (a == b)
            {
                res.set_invalid(std::to_string(a) + std::string(" is equal to ") + std::to_string(b));
            }

            results.push_back(std::move(res));
        };

        template <class A, class B>
        void equal_sequences(const std::string &check_name, const A &a, const B &b)
        {
            Result res{check_name};

            std::size_t i = 0;
            auto it_a = a.cbegin();
            auto it_b = b.cbegin();

            while (it_a != a.cend())
            {
                if (*it_a != *it_b)
                {
                    const auto str_a = std::to_string(*it_a);
                    const auto str_b = std::to_string(*it_b);
                    res.set_invalid(std::to_string(i) + "-th element of sequence `a` " + str_a +
                                            std::string(" is not equal to element of sequence `b` ") + str_b);
                }

                ++i; ++it_a; ++it_b;
            }

            results.push_back(std::move(res));
        }

        std::vector<Result> results;
    };


    struct Case
    {
        std::string name;
        Assert assert;

        virtual void operator()() = 0;
    };


    struct LambdaTestCase : public Case
    {
        using Function =  std::function<void(Assert &)>;

        LambdaTestCase(const std::string &name, Function function): function(function)
        {
            this->name = name;
        }

        void operator()() override
        {
            function(assert);
        }

        Function function;
    };


    struct Result
    {
        Result(const std::string &suite_name, const std::string &case_name,
               const Assert::Result &assert_result): assert_result(assert_result),
                                                     suite_name(suite_name),
                                                     case_name(case_name)
        {
        }

        std::string suite_name;
        std::string case_name;
        Assert::Result assert_result;
    };

public:
    TestSuite(const std::string &name): name(name)
    {
    }

    void add_case(const std::string &case_name, LambdaTestCase::Function function);

    template <class T>
    void add_case(const std::string &case_name)
    {
        auto obj = std::make_unique<T>();
        obj->name = case_name;
        cases.push_back(std::move(obj));
    }

    std::vector<Result> run();

    auto invalid_results(const std::vector<Result> &results) const;

    auto get_name() const
    {
        return name;
    }

private:
    std::string name;
    std::vector<std::unique_ptr<Case>> cases;
};


class Logger
{
public:
    virtual void log(const std::string &str) = 0;
};


void execute(const std::vector<std::shared_ptr<TestSuite>> &suites, Logger &logger);

std::shared_ptr<TestSuite> create_suite(const std::string &name);

}
