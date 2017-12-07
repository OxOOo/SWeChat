#ifndef NETWORK_TCP_SOCKET_H
#define NETWORK_TCP_SOCKET_H

#include <functional>
#include <memory>
#include <vector>
#include <sys/socket.h>

namespace swechat
{
    using namespace std;

    typedef shared_ptr<vector<char>> BinMsg;

    BinMsg CreateMsg();
    BinMsg CreateMsg(string string_msg);

    // 套接字，不对外公开构造函数
    class TCPSocket
    {
    public:
        typedef shared_ptr<TCPSocket> ptr;
        virtual ~TCPSocket();

        void Send(BinMsg msg);

        void Send(string string_msg);

        BinMsg Recv(); // 接收一条

        void Close();

    public:
        TCPSocket();
        TCPSocket(int socket_desc);

    protected:
        void initSocket();

        int socket_desc;
    };
}

#endif // NETWORK_TCP_SOCKET_H