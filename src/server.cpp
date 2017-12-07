#include <iostream>
#include <plog/Log.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#include "tcp/server.h"

using namespace std;
using namespace swechat;

int main()
{
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::debug, &consoleAppender);

    TCPServer::ptr server = TCPServer::ptr(new TCPServer("0.0.0.0", 8888));
    if (!server->Listen()) {
        return 1;
    }

    TCPSocket::ptr socket = server->WaitSocket();
    LOG_DEBUG << "New Connection";
    socket->Send("Hello World\n");
    socket->Close();
    
    server->Close();

    return 0;
}