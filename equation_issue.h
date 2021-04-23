#ifndef EQUATION_ISSUE_H
#define EQUATION_ISSUE_H

#include <iostream>
#include <queue>
#include <mutex>
#include <tuple>
#include "producer_consumer_pipe.h"

/// @brief Structure for representation of square equation problem
///
/// square problem in form: a*x^2 + b*x + c = 0
struct SquareEquanationIssue
{
    double a{};
    double b{};
    double c{};
    friend std::ostream& operator<< (std::ostream& os, const SquareEquanationIssue& val);
};

std::ostream& operator<< (std::ostream& os, const SquareEquanationIssue& val)
{
    os << "(" << val.a << " " << val.b << " " << val.c << ")";
    return os;
}

using IssuePipe = ProducerConsumerPipe<SquareEquanationIssue>;

#endif // EQUATION_ISSUE_H
