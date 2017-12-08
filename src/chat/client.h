#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H

#include <functional>
#include <thread>
#include <json11.hpp>
#include "queue.h"
#include "tcp/client.h"

namespace swechat
{
    using namespace json11;
    using namespace std;

    class ChatClient
    {
    private:
        ChatClient();

        static ChatClient* ins;
    public:
        static void createInstance();
        static ChatClient* instance();

        typedef function<void(string)> string_cb_t;
        typedef function<void(bool success)> next_cb_t; // true表示可以继续

        void BindMsg(string_cb_t msg_cb);
        void UnbindMsg();
        void BindError(string_cb_t err_cb);
        void UnbindError();

    public:
        // 连接服务器
        void Connect(string address, next_cb_t next_cb);

        void Close();

        // 登录
        void Login(string username, string password, next_cb_t next_cb);

        // 注册
        void Register(string username, string password, next_cb_t next_cb);

    private:
        TCPClient::ptr client;
        thread io_thread;
        Queue<function<void()>> task_que;
        string_cb_t msg_cb, err_cb;

        Json waitForCommand(string command_type);
    };
}

#endif // CHAT_CLIENT_H