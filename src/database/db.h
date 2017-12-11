#ifndef DATABASE_DB_H
#define DATABASE_DB_H

#include <string>
#include <vector>
#include <memory>
#include "user.h"
#include "friend_ship.h"

namespace swechat
{
    using namespace std;

    typedef shared_ptr<string> rst_t;

    class DB
    {
    public:
        DB(string db_path);
        ~DB();

        void Load();
        void Save();
        void TriggerModify();
    
    public:
        rst_t Login(string username, string password);
        rst_t Register(string username, string password);
        User::ptr FindUser(string username);
        const vector<User::ptr>& Users();

        vector<User::ptr> Friends(User::ptr u);
        FriendShip::ptr FindShip(User::ptr u1, User::ptr u2);
        rst_t CreateFriend(User::ptr u1, User::ptr u2);
        Message::ptr CreateMessage(FriendShip::ptr ship, string sender, string msg);
    
    private:
        string dbFile(string filename);
        string currentDateTime();

        string db_path;
        vector<User::ptr> users;
        vector<FriendShip::ptr> ships;
        int global_msg_id;
    };
}

#endif // DATABASE_DB_H