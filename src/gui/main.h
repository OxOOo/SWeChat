#ifndef GUI_MAIN_H
#define GUI_MAIN_H

#include "chat.h"
#include <QMainWindow>
#include <QListWidget>
#include <QLabel>

namespace swechat
{
    using namespace std;

    class MainWindow : public QMainWindow
    {
    public:
        MainWindow();
        ~MainWindow();

    private:
        void initUI();
        void initBind();

        ChatWidget* chat; // 聊天窗口
        QLabel* name_label; // 我的姓名
        QListWidget* users_list_view; // 所有用户列表
        QListWidget* friends_list_view; // 我的好友列表

        vector<user_t> users;
    };
}

#endif // GUI_MAIN_H