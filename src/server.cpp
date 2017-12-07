#include <iostream>
#include <plog/Log.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#include "chat/server.h"

using namespace std;
using namespace swechat;

int main()
{
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::debug, &consoleAppender);

    ChatServer::createInstance();
    ChatServer::instance()->Run();

    return 0;
}