#ifndef EQUATION_SOLUTION_H
#define EQUATION_SOLUTION_H

#include "equation_issue.h"
#include "producer_consumer_pipe.h"
#include <vector>
#include <optional>

/// @brief Structure for representation of square equation solution
///
/// Contains vector number of roots, there could be 0, 1 or 2 roots depends on issue.
/// Contains also information about initial issue
struct SquareEquationSolution
{
    SquareEquationIssue issue{};
    std::vector<double> roots{};
};

std::ostream& operator<< (std::ostream& os, const SquareEquationSolution& val)
{
    os << val.issue << " => ";

    switch (val.roots.size())
    {
        case 0: os << "no roots"; break;
        case 1: os << "(" << val.roots[0] << ")"; break;
        case 2: os << "(" << val.roots[0] << ", " << val.roots[1] << ")"; break;
        default: os << "calculation error occurs";
    }
    return os;
}

using SolutionPipe = ProducerConsumerPipe<SquareEquationSolution>;

#endif // EQUATION_SOLUTION_H
