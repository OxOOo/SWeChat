#ifndef DATABASE_FRIEND_SHIP_H
#define DATABASE_FRIEND_SHIP_H

#include <string>
#include <memory>
#include <vector>
#include <json11.hpp>

namespace swechat
{
    using namespace std;
    using namespace json11;

    class Message
    {
    public:
        typedef shared_ptr<Message> ptr;
        string sender;
        string msg;
        int datetime;
        bool read;
    };

    class FriendShip
    {
    public:
        typedef shared_ptr<FriendShip> ptr;

        string username1, username2;
        vector<Message::ptr> messages;

        Json to_json() const
        {
            Json::array msgs;
            for(int i = 0; i < messages.size(); i ++) {
                msgs.push_back(Json::object {
                    {"sender", messages[i]->sender},
                    {"msg", messages[i]->msg},
                    {"datetime", messages[i]->datetime},
                    {"read", messages[i]->read}
                });
            }
            return Json::object {
                {"username1", username1},
                {"username2", username2},
                {"messages", msgs}
            };
        }
        static FriendShip::ptr from_json(Json obj)
        {
            FriendShip::ptr u = make_shared<FriendShip>();
            u->username1 = obj["username1"].string_value();
            u->username2 = obj["username2"].string_value();
            for(int i = 0, j = obj["messages"].array_items().size(); i < j; i ++) {
                Message::ptr m = make_shared<Message>();
                m->sender = obj["messages"][i]["sender"].string_value();
                m->msg = obj["messages"][i]["msg"].string_value();
                m->datetime = obj["messages"][i]["datetime"].int_value();
                m->read = obj["messages"][i]["read"].bool_value();
                u->messages.push_back(m);
            }
            return u;
        }
    };
}

#endif // DATABASE_FRIEND_SHIP_H