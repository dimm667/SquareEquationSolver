#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "solver.h"
#include "argument_parser.h"
#include "solution_printer.h"

struct SolverTestCase
{
    SquareEquationIssue issue{};
    std::vector<double> result{};
};


class SolverTestFixture : public ::testing::Test, public ::testing::WithParamInterface<SolverTestCase>
{
public:

    void Verify(const SquareEquationIssue& issue, const std::vector<double>& roots)
    {
        const auto producer_id{issue_pipe.registerAsProducer()};
        issue_pipe.push({issue});
        issue_pipe.setNoMoreProducing(producer_id);
        solver();
        const auto result = std::get<0>(solution_pipe.pop())[0];
        ASSERT_THAT(result.roots, ::testing::ElementsAreArray(roots));
    }

private:
    IssuePipe issue_pipe{};
    SolutionPipe solution_pipe{};
    Solver<1> solver{issue_pipe, solution_pipe};
};

TEST_P(SolverTestFixture, parameterised_test)
{
    const auto& test_data{GetParam()};
    Verify(test_data.issue, test_data.result);
}


const std::vector<SolverTestCase> solver_test_cases{
        {{1.0, -2.0, -3.0}, {3.0, -1.0}},
        {{0.0, 4.0, -4.0}, {1.0}},
        {{1, 2, 3}, {}},
        {{0.0, 0.0, 0.0}, {}},
        {{0.0, 1.0, 0.0}, {0.0}},
        {{0.0, 1.0, 2.0}, {-2.0}},
        {{0.0, 0.0, 1.0}, {}},
        {{1.0, 0.0, 0.0}, {0.0}},
        {{1.0, 0.0, -4.0}, {2.0, -2.0}},
    };

INSTANTIATE_TEST_CASE_P(SolverTest, SolverTestFixture, ::testing::ValuesIn(solver_test_cases));


struct ArgumentParserTestCase
{
    int argc{};
    char **argv{};
    std::vector<SquareEquationIssue> result;
};

class ArgumentParserTestFixture : public ::testing::Test, public ::testing::WithParamInterface<ArgumentParserTestCase>
{
public:

    void Verify(int argc, char **argv, const std::vector<SquareEquationIssue>& expected_result)
    {
        ArgumentParser<2> parser{argc, argv, issue_pipe};

        parser();

        const auto result = std::get<0>(issue_pipe.pop(3));
        ASSERT_THAT(result, ::testing::ElementsAreArray(expected_result));
    }

private:
    IssuePipe issue_pipe{};
};

bool operator==(const SquareEquationIssue& op1, const SquareEquationIssue& op2)
{
    return (std::fabs(op1.a - op2.a) < 1e-5) &&
           (std::fabs(op1.b - op2.b) < 1e-5) &&
           (std::fabs(op1.c - op2.c) < 1e-5);
}

TEST_P(ArgumentParserTestFixture, parameterised_test)
{
    const auto& test_data{GetParam()};
    Verify(test_data.argc, test_data.argv, test_data.result);
}

char *args_1[]{{"exec"}};
char *args_2_valid[]{"exec", "11"};
char *args_2_invalid[]{"exec", "bb"};
char *args_3_valid[]{"exec", "11", "22"};
char *args_4_valid[]{"exec", "11", "22", "33"};
char *args_7_valid[]{"exec", "11", "22", "33", "44", "55", "66"};
char *args_7_invalid[]{"exec", "11", "bb", "33", "44", "55", "66"};
char *args_7_invalid_2[]{"exec", "11", "bb", "33", "44", "55", "cc"};
char *args_10_valid[]{"exec", "11", "22", "33", "44", "55", "66", "77", "88", "99"};

const std::vector<ArgumentParserTestCase> parser_test_cases{
    {1, args_1, {}},
    {2, args_2_valid, {{11.0, 0.0, 0.0}}},
    {2, args_2_invalid, {{0.0, 0.0, 0.0}}},
    {3, args_3_valid, {{11.0, 22.0, 0.0}}},
    {4, args_4_valid, {{11.0, 22.0, 33.0}}},
    {7, args_7_valid, {{11.0, 22.0, 33.0}, {44.0, 55.0, 66.0}}},
    {7, args_7_invalid, {{44.0, 55.0, 66.0}}},
    {7, args_7_invalid_2, {}},
    {10, args_10_valid, {{11.0, 22.0, 33.0}, {44.0, 55.0, 66.0}, {77.0, 88.0, 99.0}}},
};

INSTANTIATE_TEST_CASE_P(ArgumentParserTest, ArgumentParserTestFixture, ::testing::ValuesIn(parser_test_cases));

struct SolutionPrinterTestCase
{

    std::vector<SquareEquationSolution> solutions{};
    std::string result{};
};

class SolutionPrinterTestFixture : public ::testing::Test, public ::testing::WithParamInterface<SolutionPrinterTestCase>
{
public:

    void Verify(const std::vector<SquareEquationSolution>& solutions, const std::string& expected_result)
    {
        SolutionPrinter<2> printer{solution_pipe};

        solution_pipe.push(solutions);
        testing::internal::CaptureStdout();
        printer();
        std::string result = testing::internal::GetCapturedStdout();
        ASSERT_THAT(result, ::testing::StrEq(expected_result));
    }

private:
    SolutionPipe solution_pipe{};
};

TEST_P(SolutionPrinterTestFixture, parameterised_test)
{
    const auto& test_data{GetParam()};
    Verify(test_data.solutions, test_data.result);
}

const std::vector<SolutionPrinterTestCase> printer_test_cases{
    {{{{1.0, 2.0, 3.0}, {4.0, 5.0}}}, "(1 2 3) => (4, 5)\n"},
    {{{{1.0, 2.0, 3.0}, {4.0}}}, "(1 2 3) => (4)\n"},
    {{{{1.0, 2.0, 3.0}, {}}}, "(1 2 3) => no roots\n"},
    {{{{1.0, 2.0, 3.0}, {4.0, 5.0}}, {{6.1, 7.2, 8.3}, {9.4, 10.5}}}, "(1 2 3) => (4, 5)\n(6.1 7.2 8.3) => (9.4, 10.5)\n"},
    {{{{1.0, 2.0, 3.0}, {4.0, 5.0}}, {{6.1, 7.2, 8.3}, {9.4, 10.5}}, {{11, 12, 13}, {}}}, "(1 2 3) => (4, 5)\n(6.1 7.2 8.3) => (9.4, 10.5)\n(11 12 13) => no roots\n"},
};

INSTANTIATE_TEST_CASE_P(SolutionPrinterTest, SolutionPrinterTestFixture, ::testing::ValuesIn(printer_test_cases));
