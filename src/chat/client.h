#ifndef CHAT_CLIENT_H
#define CHAT_CLIENT_H

#include "tcp/client.h"

namespace swechat
{
    using namespace std;

    class ChatClient
    {
    private:
        ChatClient();

        static ChatClient* ins;
    public:
        static void createInstance();
        static ChatClient* instance();
    };
}

#endif // CHAT_CLIENT_H