#ifndef ARGUMENT_PARSER_H
#define ARGUMENT_PARSER_H

#include "equation_issue.h"
#include <string>

/// @brief Functor class for parsing command line arguments in the separate thread
///
/// Template parameter allows configure how many messages will be produced in each thread cycle
/// Arguments parsed by triplets and converted to the square equation issue
/// Produces messages to the issue pipe.
template<int parse_triple_cnt>
class ArgumentParser
{
public:

    /// @brief Default constructor
    ///
    /// @param[in] argc number of the input arguments
    /// @param[in] argv pointer to the array of the input arguments
    /// @param[in] issue_pipe reference to the issue pipe for producing messages
    ArgumentParser(int argc, char *argv[], IssuePipe& issue_pipe) :
        argc_{argc}, argv_{argv}, issue_pipe_{issue_pipe}, current_arg_{1},
        producer_id_{issue_pipe_.registerAsProducer()}
    {
    }

    /// @brief Function for running in the separate thread
    void operator()()
    {
        // this thread wil be working until all arguments will be parsed
        while(current_arg_ < argc_)
        {
            std::vector<SquareEquationIssue> issues{};
            int triple_cnt{0};

            // parsing by triplets
            for(; ((current_arg_ + 2 < argc_) && (triple_cnt < parse_triple_cnt)); current_arg_= current_arg_ + 3, ++triple_cnt)
            {
                SquareEquationIssue issue{};
                try
                {
                    issue.a = std::stod(argv_[current_arg_]);
                    issue.b = std::stod(argv_[current_arg_+1]);
                    issue.c = std::stod(argv_[current_arg_+2]);
                }
                catch(std::exception e)
                {
                    // in case of conversion error just ignore current arguments tripple
                    continue;
                }
                issues.push_back(issue);

            }
            // parsing the last arguments which is not triplet
            if(triple_cnt < parse_triple_cnt && current_arg_ > 0 && current_arg_ < argc_)
            {
                SquareEquationIssue issue{};
                try
                {
                    issue.a = std::stod(argv_[current_arg_]);
                    ++current_arg_;
                    if(current_arg_ < argc_) issue.b = std::stod(argv_[current_arg_]);
                    ++current_arg_;
                }
                catch(std::exception e)
                {
                    // in case of conversion error just ignore the last arguments
                    ++current_arg_;
                }
                issues.push_back(issue);
            }
            issue_pipe_.push(issues);
            std::this_thread::yield();
        }
        // Notify consumers that we won't produce messages anymore
        issue_pipe_.setNoMoreProducing(producer_id_);
    }

private:
    int argc_{};
    char **argv_{};
    int current_arg_{};
    IssuePipe& issue_pipe_;
    int producer_id_{};

};


#endif // ARGUMENT_PARSER_H
