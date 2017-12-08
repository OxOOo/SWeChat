#include "client.h"
#include "common.h"
#include <plog/Log.h>
#include <cassert>
#include <thread>
#include <json11.hpp>

using namespace json11;
using namespace swechat;

ChatClient* ChatClient::ins = nullptr;

void ChatClient::createInstance()
{
    LOG_ERROR_IF(ins != nullptr) << "Already created chat client instance";
    assert(ins == nullptr);
    ins = new ChatClient();
}

ChatClient* ChatClient::instance()
{
    return ins;
}

ChatClient::ChatClient()
{
    UnbindMsg();
    UnbindError();

    io_thread = thread([=]() {
        while(true)
        {
            auto task = task_que.Pop();
            task();
        }
    });
}

void ChatClient::BindMsg(string_cb_t msg_cb)
{
    this->msg_cb = msg_cb;
}
void ChatClient::UnbindMsg()
{
    this->msg_cb = [=](string){};
}
void ChatClient::BindError(string_cb_t err_cb)
{
    this->err_cb = err_cb;
}
void ChatClient::UnbindError()
{
    this->err_cb = [=](string){};
}

void ChatClient::Connect(string address, next_cb_t next_cb)
{
    task_que.Push([=]() {
        client = TCPClient::ptr(new TCPClient(address, SERVER_PORT));
        if (!client->Connect()) {
            err_cb("连接服务器失败");
            next_cb(false);
            return;
        }
        next_cb(true);
    });
}

void ChatClient::Close()
{
    task_que.Push([=]() {
        client->Close();
    });
}

void ChatClient::Login(string username, string password, next_cb_t next_cb)
{
    task_que.Push([=] {
        Json data = Json::object {
            {"command", COMMAND_LOGIN},
            {"username", username},
            {"password", password}
        };
        client->Send(data.dump());
        Json res = waitForCommand(COMMAND_LOGIN_RES);
        if (res["success"].bool_value()) {
            next_cb(true);
        } else {
            err_cb(res["msg"].string_value());
            next_cb(false);
        }
    });
}

void ChatClient::Register(string username, string password, next_cb_t next_cb)
{
    task_que.Push([=] {
        Json data = Json::object {
            {"command", COMMAND_REGISTER},
            {"username", username},
            {"password", password}
        };
        client->Send(data.dump());
        Json res = waitForCommand(COMMAND_REGISTER_RES);
        if (res["success"].bool_value()) {
            next_cb(true);
        } else {
            err_cb(res["msg"].string_value());
            next_cb(false);
        }
    });
}

Json ChatClient::waitForCommand(string command_type)
{
    while(true)
    {
        BinMsg msg = client->Recv();
        string err;
        Json data = Json::parse(msg->data(), err);
        if (data["command"] == command_type) return data;
    }
}