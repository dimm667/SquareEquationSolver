#ifndef SOLVER_H
#define SOLVER_H

#include <math.h>
#include <thread>
#include "equation_issue.h"
#include "equation_solution.h"

/// @brief Functor class for solving square equations in the separate thread
///
/// Template parameter allows configure how many equation will be solved in each thread cycle
/// Consumes messages from the issue pipe, produces messages to the solution pipe
template<int issues_count>
class Solver
{
public:

    /// @brief Default constructor
    ///
    /// @param[in] issue_pipe reference to the issue pipe for consuming messages
    /// @param[in] solution_pipe reference to the solution pipe for producing messages
    Solver(IssuePipe& issue_pipe, SolutionPipe& solution_pipe) :
    issue_pipe_{issue_pipe}, solution_pipe_{solution_pipe}, producer_id_{solution_pipe_.registerAsProducer()}
    {
    }

    /// @brief Function for running in the separate thread
    void operator()()
    {
        // this thread will be working while producer sends messages to the pipe
        while(true)
        {
            const auto [task, no_more_issues] = issue_pipe_.pop(issues_count);
            if(!task.empty())
            {
                std::vector<SquareEquanationSolution> result{};
                for(const auto& issue : task)
                {
                    result.push_back({issue, getRoots(issue)});
                }

                solution_pipe_.push(result);
            }
            else if(no_more_issues)
            {
                break;
            }

            std::this_thread::yield();
        }
        // notify solution consumer about finishing producing and terminate thread
        solution_pipe_.setNoMoreProducing(producer_id_);
    }


private:
    static constexpr double comparison_precision{1e-5};

    /// @brief Function for solving square equation
    ///
    /// @param[in] issue square equation problem to solve
    /// @returns roots of the equation (from 0 to 2)
    static std::vector<double> getRoots(const SquareEquanationIssue& issue)
    {
        std::vector<double> result{};

        if((std::fabs(issue.a) < comparison_precision) && (std::fabs(issue.b) < comparison_precision))
        {
            // not an equation, no roots
        }
        else if((std::fabs(issue.a) < comparison_precision) && (std::fabs(issue.b) > comparison_precision))
        {
            // case of linear equation
            result.push_back(-issue.c / issue.b);
        }
        else if((std::fabs(issue.a) > comparison_precision) && (std::fabs(issue.b) > comparison_precision) && (std::fabs(issue.c) < comparison_precision))
        {
            // case of simplified square equation
            result.push_back(0.0);
            result.push_back(-issue.b / issue.a);
        }
        else
        {
            const double descriminant = issue.b*issue.b - 4*issue.a*issue.c;

            // case of regular square equation
            if(std::fabs(descriminant) < comparison_precision)
            {
                // one root case
                result.push_back(-issue.b / (2.0 * issue.a));
            }
            else if(descriminant < 0.0)
            {
                // no roots case, nothing to do
            }
            else
            {
                // two roots case
                result.push_back((-issue.b + std::sqrt(descriminant))/ (2.0 * issue.a));
                result.push_back((-issue.b - std::sqrt(descriminant))/ (2.0 * issue.a));
            }
        }

        return result;
    }

    IssuePipe& issue_pipe_;
    SolutionPipe& solution_pipe_;
    int producer_id_{};
};

#endif // SOLVER_H
