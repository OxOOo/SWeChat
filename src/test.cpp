#include <iostream>
#include <plog/Log.h>
#include <plog/Appenders/ColorConsoleAppender.h>
#include "json11.hpp"

using namespace std;
using namespace json11;

int main()
{
    static plog::ColorConsoleAppender<plog::TxtFormatter> consoleAppender;
    plog::init(plog::debug, &consoleAppender);
    LOG_DEBUG << "Hello World";

    Json json_value = Json::object{
        {"int_value", 1},
        {"str", "string"}};
    LOG_INFO << json_value.dump();

    return 0;
}