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
    UnbindUsers();
    UnbindFriends();

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
    this->msg_cb = [](auto){};
}
void ChatClient::BindError(string_cb_t err_cb)
{
    this->err_cb = err_cb;
}
void ChatClient::UnbindError()
{
    this->err_cb = [](auto){};
}
void ChatClient::BindUsers(users_cb_t users_cb)
{
    this->users_cb = users_cb;
}
void ChatClient::UnbindUsers()
{
    this->users_cb = [](auto){};
}
void ChatClient::BindFriends(users_cb_t friends_cb)
{
    this->friends_cb = friends_cb;
}
void ChatClient::UnbindFriends()
{
    this->friends_cb = [](auto){};
}

void ChatClient::RecvLoop()
{
    loop_thread = thread([=]() {
        while(true) {
            BinMsg msg = client->Recv();
            string err;
            Json data = Json::parse(msg->data(), err);

            if (data["command"] == COMMAND_ERROR) {
                err_cb(data["msg"].string_value());
                continue;
            }
            if (data["command"] == COMMAND_MSG) {
                msg_cb(data["msg"].string_value());
                continue;
            }
            if (data["command"] == COMMAND_USERS) {
                vector<user_t> users;
                auto users_data = data["users"].array_items();
                for(int i = 0; i < users_data.size(); i ++) {
                    users.push_back((user_t){users_data[i]["username"].string_value(), users_data[i]["online"].bool_value()});
                }
                users_cb(users);
                continue;
            }
            if (data["command"] == COMMAND_FRIENDS) {
                vector<user_t> friends;
                auto friends_data = data["friends"].array_items();
                for(int i = 0; i < friends_data.size(); i ++) {
                    friends.push_back((user_t){friends_data[i]["username"].string_value(), friends_data[i]["online"].bool_value()});
                }
                friends_cb(friends);
                continue;
            }

            LOG_ERROR << "Unknow msg : " << data.dump();
        }
    });
}

void ChatClient::Flash()
{
    task_que.Push([=]() {
        Json data = Json::object {
            {"command", COMMAND_FLASH},
        };
        client->Send(data.dump());
    });
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