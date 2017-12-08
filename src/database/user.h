#ifndef DATABASE_USER_H
#define DATABASE_USER_H

#include <string>
#include <memory>
#include <json11.hpp>

namespace swechat
{
    using namespace std;
    using namespace json11;

    class User
    {
    public:
        typedef shared_ptr<User> ptr;

        string username;
        string password;

        Json to_json() const
        {
            return Json::object {
                {"username", username},
                {"password", password}
            };
        }
        static User::ptr from_json(Json obj)
        {
            User::ptr u = make_shared<User>();
            u->username = obj["username"].string_value();
            u->password = obj["password"].string_value();
            return u;
        }
    };
}

#endif // DATABASE_USER_H