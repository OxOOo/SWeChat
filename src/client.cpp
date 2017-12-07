#include <iostream>
#include <plog/Log.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#include "tcp/client.h"

using namespace std;
using namespace swechat;

int main()
{
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::debug, &consoleAppender);

    TCPClient::ptr client = TCPClient::ptr(new TCPClient("127.0.0.1", 8888));
    if (!client->Connect()) {
        return 1;
    }
    BinMsg msg = client->Recv();
    msg->push_back('\0');
    LOG_DEBUG << msg->data();

    return 0;
}