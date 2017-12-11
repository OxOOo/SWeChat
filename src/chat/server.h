#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include <functional>
#include <memory>
#include <thread>
#include <map>
#include "tcp/server.h"
#include "database/db.h"
#include "queue.h"
#include "file_server.h"

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
        TCPServer::ptr server, file_server;
        FileServer::ptr com_file_server;
        shared_ptr<DB> db;
        Queue<function<void()>> task_que;
        thread process_thread; // 工作线程

        map<int, thread> socket_threads;
        map<int, User::ptr> logined_users;
        map<string, int> logined_accounts;
        map<int, TCPSocket::ptr> sockets;

        map<int, thread> file_socket_threads; // socket.id -> thread
        map<int, User::ptr> file_socket_users; // socket.id -> user
        map<string, TCPSocket::ptr> file_sockets; //

        void processSocket(TCPSocket::ptr socket);
        void sendMsgTo(TCPSocket::ptr socket, string msg);
        void sendErrorTo(TCPSocket::ptr socket, string msg);
        void sendUsersTo(int socket_id);
        void sendFriendsTo(int socket_id);
        void sendChatMessagesTo(int socket_id, string username);
        void Boardcast();

        void processFileSocket(TCPSocket::ptr socket);
    };
}

#endif // CHAT_SERVER_H