#include "server.h"
#include "common.h"
#include <plog/Log.h>
#include <cassert>
#include <thread>
#include <json11.hpp>
#include <chrono>

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
    server = make_shared<TCPServer>(SERVER_ADDRESS, SERVER_PORT);
    file_server = make_shared<TCPServer>(SERVER_ADDRESS, FILE_SERVER_PORT);
    com_file_server = make_shared<FileServer>();

    assert(server->Listen());
    assert(file_server->Listen());

    process_thread = thread([=]() {
        while(true)
        {
            auto task = task_que.Pop();
            task();
        }
    });

    thread server_thread = thread([=]() {
        while(true)
        {
            TCPSocket::ptr socket = server->WaitSocket();
            task_que.Push([=]() {
                socket_threads[socket->id] =  thread([this, socket]() {
                    this->processSocket(socket);
                });
            });
        }
    });
    thread file_server_thread = thread([=]() {
        while(true)
        {
            TCPSocket::ptr socket = file_server->WaitSocket();
            task_que.Push([=]() {
                file_socket_threads[socket->id] =  thread([this, socket]() {
                    this->processFileSocket(socket);
                });
            });
        }
    });

    server_thread.join();
    file_server_thread.join();
}

void ChatServer::processSocket(TCPSocket::ptr socket)
{
    LOG_DEBUG << "processing new socket";
    task_que.Push([=]() {
        sockets[socket->id] = socket;
    });

    while(true)
    {
        BinMsg msg = socket->Recv();
        if (msg == nullptr)
        {
            socket->Close();
            break;
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
                    for(auto x : logined_users) {
                        if (x.second->username == data["username"].string_value()) sockets[x.first]->Close(); // 强制下线
                    }
                    logined_users[socket->id] = db->FindUser(data["username"].string_value());
                    logined_accounts[data["username"].string_value()] = socket->id;
                    socket->Send(res.dump());
                    Boardcast();
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
                    Boardcast();
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

            if (data["command"] == COMMAND_FLASH) {
                sendUsersTo(socket->id);
                sendFriendsTo(socket->id);
                return;
            }

            if (data["command"] == COMMAND_ADD_FRIEND) {
                rst_t rst = db->CreateFriend(logined_users[socket->id], db->FindUser(data["username"].string_value()));
                if (rst != nullptr) {
                    sendErrorTo(socket, *rst);
                } else {
                    sendMsgTo(socket, "添加好友成功");
                    sendFriendsTo(socket->id);
                    if (logined_accounts.find(data["username"].string_value()) != logined_accounts.end())
                        sendFriendsTo(logined_accounts[data["username"].string_value()]);
                }
                return;
            }

            if (data["command"] == COMMAND_QUERY_MSGS) {
                sendChatMessagesTo(socket->id, data["username"].string_value());
                return;
            }
            if (data["command"] == COMMAND_SEND_MSG) {
                auto ship = db->FindShip(logined_users[socket->id], db->FindUser(data["username"].string_value()));
                auto m = db->CreateMessage(ship, logined_users[socket->id]->username, data["msg"].string_value());
                Json::object obj = m->to_json().object_items();
                obj["command"] = COMMAND_NEW_MSG;
                obj["username"] = data["username"];
                socket->Send(Json(obj).dump());
                if (logined_accounts.find(data["username"].string_value()) != logined_accounts.end()) {
                    obj["username"] = logined_users[socket->id]->username.c_str();
                    sockets[logined_accounts[data["username"].string_value()]]->Send(Json(obj).dump());
                }
                return;
            }
            if (data["command"] == COMMAND_ACCEPT_MSG) {
                string username = data["username"].string_value();
                int msg_id = data["id"].int_value();
                auto ship = db->FindShip(logined_users[socket->id], db->FindUser(username));
                LOG_DEBUG << logined_users[socket->id]->username << " " << db->FindUser(username)->username << " " << msg_id;
                for(int i = 0; i < ship->messages.size(); i ++) {
                    if (ship->messages[i]->id == msg_id) {
                        ship->messages[i]->read = true;
                    }
                }
                db->TriggerModify();
                return;
            }
            if (data["command"] == COMMAND_REJECT_MSG) {
                string username = data["username"].string_value();
                int msg_id = data["id"].int_value();
                if (logined_accounts.find(username) != logined_accounts.end()) {
                    sendFriendsTo(socket->id);
                }
                return;
            }

            LOG_ERROR << "Unknown command : " << data.dump();
        });
    }

    task_que.Push([=]() {
        socket_threads[socket->id].join();

        socket_threads.erase(socket_threads.find(socket->id));
        sockets.erase(sockets.find(socket->id));
        if (logined_users.find(socket->id) != logined_users.end()) {
            logined_accounts.erase(logined_accounts.find(logined_users[socket->id]->username));
            logined_users.erase(logined_users.find(socket->id));
        }

        Boardcast();
    });

    LOG_DEBUG << "socket " << socket->id << " exit";
}

void ChatServer::sendMsgTo(TCPSocket::ptr socket, string msg)
{
    task_que.Push([=]() {
        Json data = Json::object {
            {"command", COMMAND_MSG},
            {"msg", msg}
        };
        socket->Send(data.dump());
    });
}

void ChatServer::sendErrorTo(TCPSocket::ptr socket, string msg)
{
    task_que.Push([=]() {
        Json data = Json::object {
            {"command", COMMAND_ERROR},
            {"msg", msg}
        };
        socket->Send(data.dump());
    });
}

void ChatServer::sendUsersTo(int socket_id)
{
    task_que.Push([=]() {
        Json::array users;
        for(int i = 0; i < db->Users().size(); i ++) {
            users.push_back(Json::object {
                {"username", db->Users()[i]->username},
                {"online", logined_accounts.find(db->Users()[i]->username) != logined_accounts.end()}
            });
        }
        Json data = Json::object {
            {"command", COMMAND_USERS},
            {"users", users},
            {"unread", 0}
        };
        sockets[socket_id]->Send(data.dump());
    });
}
void ChatServer::sendFriendsTo(int socket_id)
{
    task_que.Push([=]() {
        Json::array friends;
        string myname = logined_users[socket_id]->username;
        for(auto u : db->Friends(logined_users[socket_id])) {
            int unread = 0;
            for(auto x : db->FindShip(logined_users[socket_id], u)->messages) {
                if (x->sender != myname && !x->read) unread ++;
            }
            friends.push_back(Json::object {
                {"username", u->username},
                {"online", logined_accounts.find(u->username) != logined_accounts.end()},
                {"unread", unread}
            });
        }
        Json data = Json::object {
            {"command", COMMAND_FRIENDS},
            {"friends", friends}
        };
        sockets[socket_id]->Send(data.dump());
    });
}
void ChatServer::sendChatMessagesTo(int socket_id, string username)
{
    task_que.Push([=]() {
        FriendShip::ptr ship = db->FindShip(logined_users[socket_id], db->FindUser(username));
        Json::array msgs;
        for(int i = 0; i < ship->messages.size(); i ++) {
            msgs.push_back(ship->messages[i]->to_json());
            if (ship->messages[i]->sender == username) ship->messages[i]->read = true;
        };
        Json data = Json::object {
            {"command", COMMAND_MSGS},
            {"username", username},
            {"msgs", msgs}
        };
        db->TriggerModify();
        sockets[socket_id]->Send(data.dump());
        sendFriendsTo(socket_id);
    });
}
void ChatServer::Boardcast()
{
    for(auto x : logined_users) {
        sendUsersTo(x.first);
        sendFriendsTo(x.first);
    }
}

void ChatServer::processFileSocket(TCPSocket::ptr socket)
{
    LOG_DEBUG << "[file]processing new socket";
    while(true)
    {
        BinMsg msg = socket->Recv();
        if (msg == nullptr)
        {
            socket->Close();
            break;
        }
        LOG_INFO << "[file]" << msg->data();

        string err;
        Json data = Json::parse(msg->data(), err);
        task_que.Push([=]() {
            if (data["command"] == COMMAND_FILED_LOGIN) {
                rst_t rst = db->Login(data["username"].string_value(), data["password"].string_value());
                if (rst == nullptr) {
                    User::ptr u = db->FindUser(data["username"].string_value());
                    file_socket_users[socket->id] = u;
                    file_sockets[data["username"].string_value()] = socket;
                }
                return;
            }
            if (data["command"] == COMMAND_SEND_FILE_REQ_C2S) {
                string username = data["username"].string_value();
                if (file_sockets.find(username) == file_sockets.end()) {
                    Json::object obj = Json::object {
                        {"command", COMMAND_SEND_FILE_RES_S2C},
                        {"accept", false},
                        {"msg", "对方不在线"}
                    };
                    socket->Send(Json(obj).dump());
                } else {
                    Json::object obj = Json::object {
                        {"command", COMMAND_SEND_FILE_REQ_S2C},
                        {"username", file_socket_users[socket->id]->username},
                        {"filename", data["filename"]}
                    };
                    sockets[logined_accounts[username]]->Send(Json(obj).dump());
                }

                return;
            }
            if (data["command"] == COMMAND_SEND_FILE_RES_C2S) {
                string username = data["username"].string_value();
                Json::object obj = Json::object {
                    {"command", COMMAND_SEND_FILE_RES_S2C},
                    {"accept", data["accept"]},
                };
                if (!obj["accept"].bool_value()) {
                    obj["msg"] = "对方不同意";
                }
                file_sockets[username]->Send(Json(obj).dump());
                if (obj["accept"].bool_value()) {
                    int port = com_file_server->CreateFileServer();
                    socket->Send(Json(Json::object {
                        {"command", COMMAND_SEND_FILE_PORT},
                        {"port", port}
                    }).dump());
                    file_sockets[username]->Send(Json(Json::object {
                        {"command", COMMAND_SEND_FILE_PORT},
                        {"port", port}
                    }).dump());
                }
                return;
            }

            LOG_ERROR << "[file]Unknown command : " << data.dump();
        });
    }

    task_que.Push([=]() {
        file_socket_threads[socket->id].join();
        file_socket_threads.erase(file_socket_threads.find(socket->id));

        if (file_socket_users.find(socket->id) != file_socket_users.end()) {
            auto u = file_socket_users[socket->id];
            file_socket_users.erase(file_socket_users.find(socket->id));
            file_sockets.erase(file_sockets.find(u->username));
        }
    });

    LOG_DEBUG << "[file]socket " << socket->id << " exit";
}