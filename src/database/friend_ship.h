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
        int id;
        string sender;
        string msg;
        string datetime;
        bool read;

        Json to_json() const
        {
            return Json::object {
                {"id", id},
                {"sender", sender},
                {"msg", msg},
                {"datetime", datetime},
                {"read", read}
            };
        }
        static Message::ptr from_json(Json obj)
        {
            Message::ptr m = make_shared<Message>();
            m->id = obj["id"].int_value();
            m->sender = obj["sender"].string_value();
            m->msg = obj["msg"].string_value();
            m->datetime = obj["datetime"].string_value();
            m->read = obj["read"].bool_value();
            return m;
        }
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
                msgs.push_back(messages[i]->to_json());
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
                u->messages.push_back(Message::from_json(obj["messages"][i]));
            }
            return u;
        }
    };
}

#endif // DATABASE_FRIEND_SHIP_H