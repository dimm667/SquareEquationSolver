#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "solver.h"

struct TestCase
{
    QuadraticEquanationIssue issue{};
    std::vector<double> result{};
};


class SolverTestFixture : public ::testing::Test, public ::testing::WithParamInterface<TestCase>
{
public:

    void Verify(const QuadraticEquanationIssue& issue, const std::vector<double>& roots)
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


const std::vector<TestCase> test_cases{
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

INSTANTIATE_TEST_CASE_P(SolverTest, SolverTestFixture, ::testing::ValuesIn(test_cases));
