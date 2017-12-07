#include "client.h"
#include <plog/Log.h>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr

using namespace swechat;

TCPClient::TCPClient(string address, int port)
    : address(address), port(port)
{
}

TCPClient::~TCPClient()
{
}

bool TCPClient::Connect()
{
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    LOG_ERROR_IF(socket_desc == -1) << "Error to create socket : " << strerror(errno);
    if (socket_desc == -1) return false;

    struct sockaddr_in server;
    server.sin_addr.s_addr = inet_addr(address.c_str());
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
 
    //Connect to remote server
    auto connect_rst = connect(socket_desc , (struct sockaddr *)&server , sizeof(server));
    LOG_ERROR_IF(connect_rst < 0) << "Error to connect server : " << strerror(errno);
    if (connect_rst < 0) return false;

    initSocket();

    return true;
}