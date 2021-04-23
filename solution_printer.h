#ifndef SOLUTION_PRINTER_H
#define SOLUTION_PRINTER_H

#include <thread>
#include "equation_solution.h"

/// @brief Functor class for printing square equation solution to the standart output in the separate thread
///
/// Template parameter allows configure how many messages will be consumed in each thread cycle
/// Consumes messages from solution pipe
template<int request_count>
class SolutionPrinter
{
public:

    /// @brief Default constructor
    ///
    /// @param[in] solution_pipe reference to the solution pipe for consuming messages
    SolutionPrinter(SolutionPipe& solution_pipe) : solution_pipe_(solution_pipe)
    {}

    /// @brief Function for running in the separate thread
    void operator()()
    {
        // this thread will be working while producer sends messages to the pipe
        while(true)
        {
            const auto [result, no_more_produce] = solution_pipe_.pop(request_count);
            if(!result.empty())
            {
                for(const auto& el : result)
                {
                    std::cout << el << std::endl;
                }
            }
            else if(no_more_produce)
            {
                // There is no messages in the pipe and producer won't send any message, so thread could be termanated
                break;
            }

            std::this_thread::yield();
        }
    }

private:
    SolutionPipe& solution_pipe_;
};

#endif // SOLUTION_PRINTER_H
