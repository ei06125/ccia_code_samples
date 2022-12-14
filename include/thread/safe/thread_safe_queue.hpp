#ifndef THREAD_SAFE_QUEUE_HPP
#define THREAD_SAFE_QUEUE_HPP

#include <condition_variable>
#include <mutex>
#include <queue>

namespace ts
{

    template <typename T>
    class thread_safe_queue
    {

    public:
        // thread_safe_queue();
        // thread_safe_queue(const thread_safe_queue &);
        // thread_safe_queue &operator=(const thread_safe_queue &) = delete;

        void push(T new_value)
        {
            std::lock_guard<std::mutex> lk(mut);
            data_queue.push(new_value);
            data_cond.notify_one();
        }

        T pop()
        {
            std::unique_lock<std::mutex> lk(mut);
            data_cond.wait(lk, [this]
                           { return !data_queue.empty(); });
            T res = data_queue.front();
            data_queue.pop();
            return res;
        }

        bool empty()
        {
            std::lock_guard<std::mutex> lk(mut);
            return data_queue.empty();
        }

    private:
        std::mutex mut;
        std::queue<T> data_queue;
        std::condition_variable data_cond;
    };

} // namespace ts

#endif /* THREAD_SAFE_QUEUE_HPP */