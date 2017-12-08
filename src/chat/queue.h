#ifndef CHAT_QUEUE_H
#define CHAT_QUEUE_H

#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>

namespace swechat
{
    using namespace std;

    template<typename Task>
    class Queue
    {
    public:
    
        void Push(Task task)
        {
            unique_lock<mutex> lock(m_mutex);
            m_queue.push(task);
            m_cond.notify_one();
        }

        Task Pop()
        {
            unique_lock<mutex> lock(m_mutex);
            while (m_queue.empty())
            {
                m_cond.wait_for(lock, std::chrono::seconds(1));
            }

            Task t = m_queue.front();
            m_queue.pop();
            return t;
        }
        
    private:
        mutex m_mutex;
        condition_variable m_cond;
        queue<Task> m_queue;
    };
}

#endif // CHAT_QUEUE_H