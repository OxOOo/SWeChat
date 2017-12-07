#ifndef CHAT_SERVER_H
#define CHAT_SERVER_H

#include <memory>
#include "tcp/server.h"

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

        void processSocket(TCPSocket::ptr socket);
    };
}

#endif // CHAT_SERVER_H