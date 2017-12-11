#ifndef CHAT_FILE_CLIENT_H
#define CHAT_FILE_CLIENT_H

#include "tcp/client.h"
#include "queue.h"
#include <functional>
#include <memory>
#include <map>
#include <thread>

namespace swechat
{
    using namespace std;

    class FileClient
    {
    public:
        typedef shared_ptr<FileClient> ptr;
        FileClient();
        ~FileClient();

        typedef function<void(int sent, int total)> progress_cb_t;
        typedef function<void()> finished_cb_t;

        void SendFile(int port, string send_filename, progress_cb_t progress_cb, finished_cb_t finished_cb);
        void RecvFile(int port, string save_filename, progress_cb_t progress_cb, finished_cb_t finished_cb);

    private:
        thread task_thread;
        Queue<function<void()> > task_que;
        map<int, thread> running_threads; // port -> thread
    };
}

#endif // CHAT_FILE_CLIENT_H