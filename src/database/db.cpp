#include "db.h"
#include <sys/stat.h>
#include <string>
#include <fstream>
#include <streambuf>
#include <cassert>
#include <ctime>

using namespace swechat;
using namespace std;

DB::DB(string db_path)
    : db_path(db_path)
{
    Load();
}

DB::~DB()
{
    Save();
}

rst_t DB::Login(string username, string password)
{
    User::ptr user = nullptr;
    for(auto u : users)
    {
        if (u->username == username) user = u;
    }
    if (user == nullptr) return make_shared<string>("用户不存在");
    if (user->password != password) return make_shared<string>("用户名或密码不正确");
    return nullptr;
}
rst_t DB::Register(string username, string password)
{
    User::ptr user = nullptr;
    for(auto u : users)
    {
        if (u->username == username) user = u;
    }
    if (user != nullptr) return make_shared<string>("用户已存在");
    user = make_shared<User>();
    user->username = username;
    user->password = password;
    users.push_back(user);

    Save();
    return nullptr;
}

User::ptr DB::FindUser(string username)
{
    for(auto u : users)
    {
        if (u->username == username) return u;
    }
    return nullptr;
}

const vector<User::ptr>& DB::Users()
{
    return users;
}

vector<User::ptr> DB::Friends(User::ptr u)
{
    vector<User::ptr> friends;
    for(int i = 0; i < ships.size(); i ++) {
        if (ships[i]->username1 == u->username) {
            friends.push_back(FindUser(ships[i]->username2));
        } else if (ships[i]->username2 == u->username) {
            friends.push_back(FindUser(ships[i]->username1));
        }
    }
    return friends;
}

FriendShip::ptr DB::FindShip(User::ptr u1, User::ptr u2)
{
    for(int i = 0; i < ships.size(); i ++) {
        if (ships[i]->username1 == u1->username && ships[i]->username2 == u2->username) {
            return ships[i];
        }
        if (ships[i]->username1 == u2->username && ships[i]->username2 == u1->username) {
            return ships[i];
        }
    }
    return nullptr;
}

rst_t DB::CreateFriend(User::ptr u1, User::ptr u2)
{
    if (u1 == nullptr || u2 == nullptr) {
        return make_shared<string>("用户不存在");
    }
    if (u1->username == u2->username) {
        return make_shared<string>("不能添加自己为好友");
    }
    FriendShip::ptr s = FindShip(u1, u2);
    if (s != nullptr) {
        return make_shared<string>("已经是好友");
    }
    s = make_shared<FriendShip>();
    s->username1 = u1->username;
    s->username2 = u2->username;
    ships.push_back(s);

    Save();
    return nullptr;
}

Message::ptr DB::CreateMessage(FriendShip::ptr ship, string sender, string msg)
{
    Message::ptr m = make_shared<Message>();
    m->id = global_msg_id ++;
    m->sender = sender;
    m->msg = msg;
    m->datetime = currentDateTime();
    m->read = false;
    ship->messages.push_back(m);

    Save();
    return m;
}

void DB::Load()
{
    {
        global_msg_id = 0;
        ifstream fd(dbFile("vars.json"));
        if (fd) {
            string str((istreambuf_iterator<char>(fd)), istreambuf_iterator<char>());
            string err;
            Json data = Json::parse(str, err);
            assert(data.is_object());
            global_msg_id = data["global_msg_id"].int_value();
        }
    }

    {
        users.clear();
        ifstream fd(dbFile("users.json"));
        if (fd) {
            string str((istreambuf_iterator<char>(fd)), istreambuf_iterator<char>());
            string err;
            Json arr = Json::parse(str, err);
            assert(arr.is_array());
            for(int i = 0, j = arr.array_items().size(); i < j; i ++) {
                users.push_back(User::from_json(arr[i]));
            }
        }
    }

    {
        ships.clear();
        ifstream fd(dbFile("friend_ships.json"));
        if (fd) {
            string str((istreambuf_iterator<char>(fd)), istreambuf_iterator<char>());
            string err;
            Json arr = Json::parse(str, err);
            assert(arr.is_array());
            for(int i = 0, j = arr.array_items().size(); i < j; i ++) {
                ships.push_back(FriendShip::from_json(arr[i]));
            }
        }
    }
}
void DB::Save()
{
    mkdir(db_path.c_str(), 0755);

    {
        ofstream fd(dbFile("vars.json"));
        Json data = Json::object {
            {"global_msg_id", global_msg_id}
        };
        fd << data.dump();
    }

    {
        ofstream fd(dbFile("users.json"));
        Json::array arr;
        for(auto u: users) {
            arr.push_back(u->to_json());
        }
        fd << Json(arr).dump();
    }

    {
        ofstream fd(dbFile("friend_ships.json"));
        Json::array arr;
        for(auto u: ships) {
            arr.push_back(u->to_json());
        }
        fd << Json(arr).dump();
    }
}

void DB::TriggerModify()
{
    Save();
}

string DB::dbFile(string filename)
{
    if (db_path[db_path.length()-1] == '/')
        return db_path + filename;
    return db_path + "/" + filename;
}

string DB::currentDateTime()
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[1024];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%Y-%m-%d %I:%M:%S", timeinfo);
    return string(buffer);
}