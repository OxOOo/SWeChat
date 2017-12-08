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
        thread process_thread; // 工作线程

        map<int, thread> socket_threads;
        map<int, User::ptr> logined_users;
        map<string, int> logined_accounts;
        map<int, TCPSocket::ptr> sockets;

        void processSocket(TCPSocket::ptr socket);
        void sendUsersTo(int socket_id);
        void sendFriendsTo(int socket_id);
        void Boardcast();
    };
}

#endif // CHAT_SERVER_H