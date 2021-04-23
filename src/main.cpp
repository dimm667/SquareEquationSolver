#include "solver.h"
#include "argument_parser.h"
#include "solution_printer.h"
#include <thread>
#include <chrono>

int main(int argc, char *argv[])
{

    constexpr int issues_count_per_iteration{10};

    IssuePipe issue_pipe{};
    SolutionPipe solution_pipe{};
    ArgumentParser<10> argument_parser{argc, argv, issue_pipe};
    SolutionPrinter<10> solution_printer{solution_pipe};

    int num_threads = std::thread::hardware_concurrency();
    std::vector<std::thread> thread_pool{};

    // this thread produces issues
    thread_pool.emplace_back(argument_parser);

    // this thread consumes solutions
    thread_pool.emplace_back(solution_printer);

    // these threads consume issues and produce solutions
    for(int i{2}; i < num_threads; ++i)
    {
        thread_pool.emplace_back(Solver<issues_count_per_iteration>{issue_pipe, solution_pipe});
    }

    for(int i{0}; i < num_threads; ++i)
    {
        thread_pool[i].join();
    }
}
