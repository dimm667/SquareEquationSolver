#ifndef PRODUCER_CONSUMER_PIPE_H
#define PRODUCER_CONSUMER_PIPE_H

#include <algorithm>


/// @brief Class representaton of the producer-consumer communication pipe

template<class T>
class ProducerConsumerPipe
{
public:

    /// @brief function for getting number of messages in the pipe
    std::size_t size() const
    {
        return data_.size();
    }

    /// @brief function for registering producer for this pipe
    ///
    /// Function should be called from main thread
    /// @returns id of producer for current pipe
    int registerAsProducer()
    {
        no_more_data_produced_.push_back(false);
        return no_more_data_produced_.size() - 1;
    }

    /// @brief function for notifying that current producer finished produce data
    ///
    /// @param[in] producer_id id of the producer
    void setNoMoreProducing(int producer_id)
    {
        std::lock_guard<std::mutex> lk(m_);
        no_more_data_produced_[producer_id] = true;
    }

    /// @brief function for sending data to the pipe
    ///
    /// @param[in] data data to send
    void push(const std::vector<T>& data)
    {
        std::lock_guard<std::mutex> lk(m_);
        for(const auto& el : data)
        {
            data_.push(el);
        }
    }

    /// @brief function for receiving data from the pipe
    ///
    /// @param[in] request_count number of requested messages
    /// @returns requested number of messages (or less if there is no more data in the pipe) and flag that data will no longer be produced
    std::tuple<std::vector<T>, bool> pop(const std::size_t request_count = 1)
    {
        std::vector<T> result{};
        std::lock_guard<std::mutex> lk(m_);
        bool no_more_producing = std::all_of(no_more_data_produced_.begin(),
                                             no_more_data_produced_.end(),
                                             [](bool val){return val;});

        for(int idx{0}; idx < request_count; ++idx)
        {
            if(data_.empty()) break;

            result.push_back(data_.front());
            data_.pop();
        }

        return std::make_tuple(result, no_more_producing);
    }

private:
    std::mutex m_{};
    std::vector<bool> no_more_data_produced_{};
    std::queue<T> data_{};

};

#endif // PRODUCER_CONSUMER_PIPE_H
