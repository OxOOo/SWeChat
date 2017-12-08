#include <iostream>
#include <plog/Log.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#include <QApplication>
#include "chat/client.h"
#include "gui/welcome.h"
#include "gui/main.h"

using namespace std;
using namespace swechat;

int main(int argc, char* argv[])
{
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::debug, &consoleAppender);

    QApplication app(argc, argv);
    ChatClient::createInstance();

    WelcomeWindow welcome;
    int rst = welcome.exec();
    if (rst == 0)
    {
        return 0;
    }

    MainWindow w;
    w.show();

    return app.exec();
}