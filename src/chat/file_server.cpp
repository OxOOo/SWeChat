#include "file_server.h"
#include "common.h"
#include <cstdlib>

using namespace swechat;

FileServer::FileServer()
{
    task_thread = thread([=]() {
        while(true)
        {
            auto task = task_que.Pop();
            task();
        }
    });
}

FileServer::~FileServer()
{

}

int FileServer::CreateFileServer()
{
    int port = 0;
    TCPServer::ptr server;

    while(true)
    {
        const int MIN = 10000, MAX = 99999;
        port = rand() % (MAX-MIN+1) + MIN;
        server = make_shared<TCPServer>(SERVER_ADDRESS, port);
        if (server->Listen()) break;
    }
    running_threads[port] = thread([=]() {
        TCPSocket::ptr a = server->WaitSocket();
        TCPSocket::ptr b = server->WaitSocket();
        server->Close();

        thread at = thread([=]() {
            while(true)
            {
                auto data = a->Recv();
                if (data == nullptr) break;
                b->Send(data);
            }
        });
        thread bt = thread([=]() {
            while(true)
            {
                auto data = b->Recv();
                if (data == nullptr) break;
                a->Send(data);
            }
        });

        at.join();
        bt.join();
        a->Close();
        b->Close();

        task_que.Push([=]() {
            running_threads[port].join();
            running_threads.erase(running_threads.find(port));
        });
    });

    return port;
}