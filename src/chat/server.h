#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include <functional>
#include <memory>
#include <thread>
#include <map>
#include "tcp/server.h"
#include "database/db.h"
#include "queue.h"

namespace swechat
{
    using namespace std;

    class ChatServer
    {
    private:
        ChatServer();

        static ChatServer* ins;
    public:
        static void createInstance();
        static ChatServer* instance();

    public:
        void Run();

    private:
        TCPServer::ptr server;
        shared_ptr<DB> db;
        Queue<function<void()>> task_que;
        thread process_thread;
        map<int, thread> socket_threads;

        void processSocket(TCPSocket::ptr socket);
    };
}

#endif // CHAT_SERVER_H