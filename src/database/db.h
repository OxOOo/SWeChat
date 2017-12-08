#ifndef DATABASE_DB_H
#define DATABASE_DB_H

#include <string>
#include <vector>
#include <memory>
#include "user.h"

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
    
    public:
        rst_t Login(string username, string password);
        rst_t Register(string username, string password);
    
    private:
        string dbFile(string filename);

        string db_path;
        vector<User::ptr> users;
    };
}

#endif // DATABASE_DB_H