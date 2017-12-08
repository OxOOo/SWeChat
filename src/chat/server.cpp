#include "server.h"
#include "common.h"
#include <plog/Log.h>
#include <cassert>
#include <thread>
#include <json11.hpp>

using namespace json11;
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
    db = make_shared<DB>(SERVER_DATABASE_PATH);
}

void ChatServer::Run()
{
    server = TCPServer::ptr(new TCPServer(SERVER_ADDRESS, SERVER_PORT));
    assert(server->Listen());

    process_thread = thread([=]() {
        while(true)
        {
            auto task = task_que.Pop();
            task();
        }
    });

    while(true)
    {
        TCPSocket::ptr socket = server->WaitSocket();
        socket_threads[socket->id] =  thread([this, socket]() {
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
        if (msg == nullptr)
        {
            socket->Close();
            return;
        }
        LOG_INFO << msg->data();

        string err;
        Json data = Json::parse(msg->data(), err);
        task_que.Push([=]() {

            if (data["command"] == COMMAND_LOGIN) {
                rst_t rst = db->Login(data["username"].string_value(), data["password"].string_value());
                if (rst == nullptr) {
                    Json res = Json::object {
                        {"command", COMMAND_LOGIN_RES},
                        {"success", true}
                    };
                    socket->Send(res.dump());
                } else {
                    Json res = Json::object {
                        {"command", COMMAND_LOGIN_RES},
                        {"success", false},
                        {"msg", *rst}
                    };
                    socket->Send(res.dump());
                }
                return;
            }

            if (data["command"] == COMMAND_REGISTER) {
                rst_t rst = db->Register(data["username"].string_value(), data["password"].string_value());
                if (rst == nullptr) {
                    Json res = Json::object {
                        {"command", COMMAND_REGISTER_RES},
                        {"success", true}
                    };
                    socket->Send(res.dump());
                } else {
                    Json res = Json::object {
                        {"command", COMMAND_REGISTER_RES},
                        {"success", false},
                        {"msg", *rst}
                    };
                    socket->Send(res.dump());
                }
                return;
            }

            LOG_ERROR << "Unknow command : " << data.dump();
        });
    }
}