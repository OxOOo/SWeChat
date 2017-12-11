#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H

#include <vector>
#include <functional>
#include <thread>
#include <json11.hpp>
#include "queue.h"
#include "tcp/client.h"
#include "file_client.h"

namespace swechat
{
    using namespace json11;
    using namespace std;

    struct user_t
    {
        string username;
        bool online;
        int unread;
    };
    struct message_t
    {
        int id;
        string sender;
        string msg;
        string datetime;
    };

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
        typedef function<void(vector<user_t>)> users_cb_t;
        typedef function<void(string,message_t)> chat_msg_cb_t;
        typedef function<void(string,vector<message_t>)> chat_msgs_cb_t;
        typedef function<void(string username, string filename, next_cb_t)> file_cb_t;

        void BindMsg(string_cb_t msg_cb);
        void UnbindMsg();
        void BindError(string_cb_t err_cb);
        void UnbindError();
        void BindUsers(users_cb_t users_cb); // 所有用户列表
        void UnbindUsers();
        void BindFriends(users_cb_t friends_cb); // 好友列表
        void UnbindFriends();
        void BindChatMsg(chat_msg_cb_t chat_msg_cb);
        void UnbindChatMsg();
        void BindChatMsgs(chat_msgs_cb_t chat_msgs_cb);
        void UnbindChatMsgs();

        void RecvLoop();
        void Flash();

        void AddFriend(string username); // 添加好友

        void QueryMsgs(string username); // 查询消息
        void SendMsg(string username, string msg); // 发送消息
        void AcceptMsg(string username, int msg_id); // 接受消息
        void RejectMsg(string username, int msg_id); // 未接受消息

    public:
        void SendFile(string username, string filename, next_cb_t next_cb);
        void BindFile(file_cb_t file_cb);

        void StartSendFile(string send_filename, FileClient::progress_cb_t progress_cb, FileClient::finished_cb_t finished_cb);
        void StartRecvFile(string save_filename, FileClient::progress_cb_t progress_cb, FileClient::finished_cb_t finished_cb);

    public:
        // 连接服务器
        void Connect(string address, next_cb_t next_cb);

        void Close();

        // 登录
        void Login(string username, string password, next_cb_t next_cb);

        // 注册
        void Register(string username, string password, next_cb_t next_cb);

        string LoginedUsername();

    private:
        TCPClient::ptr client, file_client;
        FileClient::ptr com_file_client;
        string logined_username, logined_password;
        thread io_thread, loop_thread;
        Queue<function<void()>> task_que;
        string_cb_t msg_cb, err_cb;
        users_cb_t users_cb, friends_cb;
        chat_msg_cb_t chat_msg_cb;
        chat_msgs_cb_t chat_msgs_cb;
        file_cb_t file_cb;

        Json waitForCommand(string command_type, TCPClient::ptr c);
    };
}

#endif // CHAT_CLIENT_H