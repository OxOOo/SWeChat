#ifndef NETWORK_TCP_SERVER_H
#define NETWORK_TCP_SERVER_H

#include <string>
#include <memory>
#include "socket.h"

namespace swechat
{
    using namespace std;

    class TCPServer
    {
    public:
        typedef shared_ptr<TCPServer> ptr;
        TCPServer(string address, int port, int backlog = 1024);
        virtual ~TCPServer();

        // return true if success
        bool Listen();

        // 返回一个客户端的套接字，阻塞
        TCPSocket::ptr WaitSocket();

        void Close();

    private:
        string address;
        int port;
        int backlog;
        int socket_desc;
        int socket_id_min;
    };
}

#endif // NETWORK_TCP_SERVER_H