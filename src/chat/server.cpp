#include "server.h"
#include "common.h"
#include <plog/Log.h>
#include <cassert>
#include <thread>

using namespace swechat;

ChatServer* ChatServer::ins = nullptr;

void ChatServer::createInstance()
{
    LOG_ERROR_IF(ins != nullptr) << "Already created chat server instance";
    assert(ins == nullptr);
    ins = new ChatServer();
}

ChatServer* ChatServer::instance()
{
    return ins;
}

ChatServer::ChatServer()
{
}

void ChatServer::Run()
{
    server = TCPServer::ptr(new TCPServer(SERVER_ADDRESS, SERVER_PORT));
    assert(server->Listen());

    while(true)
    {
        TCPSocket::ptr socket = server->WaitSocket();
        std::thread t([this, socket]() {
            this->processSocket(socket);
        });
    }
}

void ChatServer::processSocket(TCPSocket::ptr socket)
{
    LOG_DEBUG << "processing new socket";
    while(true)
    {
        BinMsg msg = socket->Recv();
        LOG_INFO << msg->data();
    }
}