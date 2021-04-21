#ifndef EQUATION_SOLUTION_H
#define EQUATION_SOLUTION_H

#include "equation_issue.h"
#include "producer_consumer_pipe.h"
#include <vector>
#include <optional>

/// @brief Structure for representation of quadratic equation solution
///
/// Contains vector number of roots, there could be 0, 1 or 2 roots depends on issue.
/// Contains also information about initial issue
struct QuadraticEquanationSolution
{
    QuadraticEquanationIssue issue{};
    std::vector<double> roots{};
    friend std::ostream& operator<< (std::ostream& os, const QuadraticEquanationSolution& val);
};

std::ostream& operator<< (std::ostream& os, const QuadraticEquanationSolution& val)
{
    os << val.issue << " => ";

    switch (val.roots.size())
    {
        case 0: os << "no roots"; break;
        case 1: os << "(" << val.roots[0] << ")"; break;
        case 2: os << "(" << val.roots[0] << " " << val.roots[1] << ")"; break;
        default: os << "wrong roots";
    }
    return os;
}

using SolutionPipe = ProducerConsumerPipe<QuadraticEquanationSolution>;

#endif // EQUATION_SOLUTION_H
