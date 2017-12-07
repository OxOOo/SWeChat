#include "client.h"
#include <plog/Log.h>
#include <cassert>

using namespace swechat;

ChatClient* ChatClient::ins = nullptr;

ChatClient::ChatClient()
{
    
}

void ChatClient::createInstance()
{
    LOG_ERROR_IF(ins != nullptr) << "Already created chat client instance";
    assert(ins == nullptr);
    ins = new ChatClient();
}

ChatClient* ChatClient::instance()
{
    return ins;
}