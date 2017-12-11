#include "file_client.h"
#include "common.h"
#include <cstdio>
#include <cassert>

using namespace std;
using namespace swechat;

FileClient::FileClient()
{
    task_thread = thread([=]() {
        while(true)
        {
            auto task = task_que.Pop();
            task();
        }
    });
}

FileClient::~FileClient()
{

}

void FileClient::SendFile(int port, string send_filename, progress_cb_t progress_cb, finished_cb_t finished_cb)
{
    running_threads[port] = thread([=]() {
        FILE* fd = fopen(send_filename.c_str(), "rb");
        char buffer[1024*4];

        fseek(fd, 0, SEEK_END);
        int total = ftell(fd);
        fseek(fd, 0, SEEK_SET);

        TCPClient::ptr client = make_shared<TCPClient>(SERVER_ADDRESS, port);
        assert(client->Connect());

        client->Send(CreateMsg(&total, sizeof(total)));

        int sent = 0;
        while(sent < total)
        {
            progress_cb(sent, total);
            int x = fread(buffer, sizeof(char), sizeof(buffer), fd);
            sent += x;
            client->Send(CreateMsg(buffer, x));
        }

        client->Close();

        fclose(fd);

        finished_cb();

        task_que.Push([=]() {
            running_threads[port].join();
            running_threads.erase(running_threads.find(port));
        });
    });
}

void FileClient::RecvFile(int port, string save_filename, progress_cb_t progress_cb, finished_cb_t finished_cb)
{
    running_threads[port] = thread([=]() {
        FILE* fd = fopen(save_filename.c_str(), "wb");

        TCPClient::ptr client = make_shared<TCPClient>(SERVER_ADDRESS, port);
        assert(client->Connect());

        auto total_data = client->Recv();
        int total = *(int*)total_data->data();

        int sent = 0;
        while(sent < total)
        {
            progress_cb(sent, total);
            auto data = client->Recv();
            int x = fwrite(data->data(), sizeof(char), data->size(), fd);
            sent += x;
        }

        client->Close();

        fclose(fd);

        finished_cb();

        task_que.Push([=]() {
            running_threads[port].join();
            running_threads.erase(running_threads.find(port));
        });
    });
}