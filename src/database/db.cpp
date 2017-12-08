#include "db.h"
#include <sys/stat.h>
#include <string>
#include <fstream>
#include <streambuf>
#include <cassert>

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

void DB::Load()
{
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
}
void DB::Save()
{
    mkdir(db_path.c_str(), 0755);

    {
        ofstream fd(dbFile("users.json"));
        Json::array arr;
        for(auto u: users) {
            arr.push_back(u->to_json());
        }
        fd << Json(arr).dump();
    }
}

string DB::dbFile(string filename)
{
    if (db_path[db_path.length()-1] == '/')
        return db_path + filename;
    return db_path + "/" + filename;
}