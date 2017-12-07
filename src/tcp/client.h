#ifndef NETWORK_TCP_CLIENT_H
#define NETWORK_TCP_CLIENT_H

#include <memory>
#include "socket.h"

namespace swechat
{
    using namespace std;

    class TCPClient : public TCPSocket
    {
    public:
        typedef shared_ptr<TCPClient> ptr;
        TCPClient(string address, int port);
        virtual ~TCPClient();

        // return true if success
        bool Connect();

    private:
        string address;
        int port;
    };
}

#endif // NETWORK_TCP_CLIENT_H