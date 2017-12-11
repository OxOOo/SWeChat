#ifndef CHAT_FILE_SERVER_H
#define CHAT_FILE_SERVER_H

#include "tcp/server.h"
#include "queue.h"
#include <functional>
#include <memory>
#include <map>

namespace swechat
{
    using namespace std;

    class FileServer
    {
    public:
        typedef shared_ptr<FileServer> ptr;
        FileServer();
        ~FileServer();

        // 创建
        int CreateFileServer();

    private:
        thread task_thread;
        Queue<function<void()> > task_que;
        map<int, thread> running_threads; // port -> thread
    };
}

#endif // CHAT_FILE_SERVER_H