#include "server.h"
#include <plog/Log.h>
#include <cassert>
#include <sys/socket.h>
#include <arpa/inet.h> //inet_addr

using namespace swechat;

TCPServer::TCPServer(string address, int port, int backlog)
    : address(address), port(port), backlog(backlog), socket_desc(-1)
{
    socket_id_min = 0;
}

TCPServer::~TCPServer()
{
}

bool TCPServer::Listen()
{
    struct sockaddr_in server;

    //Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    LOG_ERROR_IF(socket_desc == -1) << "Could not create socket : " << strerror(errno);
    if (socket_desc == -1) return false;
    LOG_INFO << "Create socket successed";

    int flag = 1;
    setsockopt(socket_desc, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int));

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(address.c_str());
    server.sin_port = htons(port);

    //Bind
    auto bind_rst = bind(socket_desc, (struct sockaddr *)&server, sizeof(server));
    LOG_ERROR_IF(bind_rst < 0) << "Bind failed : " << strerror(errno);
    if (bind_rst < 0) return false;
    LOG_INFO << "Bind socket successed";

    auto listen_rst = listen(socket_desc, backlog);
    LOG_ERROR_IF(listen_rst < 0) << "Listen failed : " << strerror(errno);
    if (listen_rst < 0) return false;
    LOG_INFO << "Success listen on " << address << ":" << port;

    return true;
}

TCPSocket::ptr TCPServer::WaitSocket()
{
    LOG_INFO << "Waiting for connections";

    struct sockaddr_in client;
    int c = sizeof(struct sockaddr_in);
    int new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
    LOG_ERROR_IF(new_socket < 0) << "ERROR CONNECTION : " << strerror(errno);
    if (new_socket < 0) return nullptr;

    LOG_INFO << "New Connection(" << new_socket << ") from " << inet_ntoa(client.sin_addr);

    char *client_ip = inet_ntoa(client.sin_addr);
    int client_port = ntohs(client.sin_port);

    TCPSocket::ptr client_ptr = make_shared<TCPSocket>(new_socket, socket_id_min);
    socket_id_min ++;
    return client_ptr;
}

void TCPServer::Close()
{
    int rst = shutdown(socket_desc, 2);
    LOG_ERROR_IF(rst != 0) << "Close TCP server error : " << strerror(errno);
}