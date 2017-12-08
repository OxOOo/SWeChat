#include "socket.h"
#include <plog/Log.h>
#include <arpa/inet.h> //inet_addr
#include <cstdio>
#include <unistd.h>
#include <netinet/tcp.h>
#include <cstring>
#include <sys/socketvar.h>

using namespace swechat;

BinMsg swechat::CreateMsg()
{
    return make_shared<vector<char>>();
}

BinMsg swechat::CreateMsg(string string_msg)
{
    auto msg = make_shared<vector<char>>();
    msg->resize(string_msg.length()+1);
    memcpy(msg->data(), string_msg.data(), string_msg.length());
    msg->at(string_msg.length()) = '\0';
    return msg;
}

TCPSocket::~TCPSocket()
{
}

TCPSocket::TCPSocket()
    : socket_desc(-1), id(-1)
{
}

TCPSocket::TCPSocket(int socket_desc, int id)
    : id(id)
{
    this->socket_desc = socket_desc;
    initSocket();
}

void TCPSocket::Send(BinMsg msg)
{
    int len = msg->size();
    write(socket_desc, &len, sizeof(int));
    write(socket_desc, msg->data(), msg->size());
}

void TCPSocket::Send(string string_msg)
{
    Send(CreateMsg(string_msg));
}

BinMsg TCPSocket::Recv()
{
    int len;
    auto len_rst = recv(socket_desc, &len, sizeof(int), MSG_WAITALL);
    LOG_ERROR_IF(len_rst < 0) << "ERROR on recv len : " << strerror(errno);
    if (len_rst <= 0) return nullptr;

    BinMsg msg = CreateMsg();
    msg->resize(len);
    auto data_rst = recv(socket_desc, msg->data(), len, MSG_WAITALL);
    LOG_ERROR_IF(data_rst < 0) << "ERROR on recv data : " << strerror(errno);
    if (data_rst < 0) return nullptr;

    return msg;
}

void TCPSocket::initSocket()
{
    int flag = 1; 
    setsockopt(socket_desc, IPPROTO_TCP, TCP_NODELAY, (char *)&flag, sizeof(int));
}

void TCPSocket::Close()
{
    int rst = shutdown(socket_desc, 2);
    LOG_ERROR_IF(rst != 0) << "Close TCP socket error : " << strerror(errno);
}