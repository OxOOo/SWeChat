#include "main.h"
#include "chat/client.h"
#include <QDesktopWidget>
#include <QApplication>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QTabWidget>
#include <QStatusBar>
#include <QDebug>
#include <QMessageBox>

using namespace swechat;

MainWindow::MainWindow()
{
    initUI();
    initBind();

    ChatClient::instance()->RecvLoop();
    ChatClient::instance()->Flash();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [=]() {
        if (!main_task_que.isEmpty()) {
            auto task = main_task_que.head();
            main_task_que.pop_front();
            task();
        }
    });
    timer->start(200);
}

MainWindow::~MainWindow()
{

}

void MainWindow::initUI()
{
    setWindowTitle("SWeChat");
    resize(800, 600);
    QDesktopWidget *desktop = QApplication::desktop();
    move((desktop->width() - this->width()) / 2, (desktop->height() - this->height()) / 2);

    QVBoxLayout* right_layout = new QVBoxLayout();
    right_layout->addWidget(name_label = new QLabel(QString::fromStdString("我的名字：" + ChatClient::instance()->LoginedUsername())));
    // right_layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));

    QTabWidget* tab_widget = new QTabWidget(this);
    tab_widget->addTab(friends_list_view = new QListWidget(this), "我的好友");
    tab_widget->addTab(users_list_view = new QListWidget(this), "所有用户");
    right_layout->addWidget(tab_widget);

    QSizePolicy tab_size_policy = tab_widget->sizePolicy();
    tab_size_policy.setVerticalPolicy(QSizePolicy::Expanding);
    tab_widget->setSizePolicy(tab_size_policy);

    QHBoxLayout* main_layout = new QHBoxLayout();
    main_layout->addWidget(chat = new ChatWidget(this), 3);
    // main_layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum));
    main_layout->addLayout(right_layout, 1);

    QWidget* central_widget = new QWidget(this);
    setCentralWidget(central_widget);

    central_widget->setLayout(main_layout);

    connect(friends_list_view, &QListWidget::itemDoubleClicked, [=](QListWidgetItem * item) {
        QString username = QString::fromStdString(friends[friends_list_view->currentRow()].username);
        chat->setUser(username);
        chat->setEnabled(true);
    });
    connect(users_list_view, &QListWidget::itemDoubleClicked, [=](QListWidgetItem * item) {
        QString username = QString::fromStdString(users[users_list_view->currentRow()].username);
        if (QMessageBox::question(this, "好友请求确认", "确定添加[" + username + "]为好友？", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes) {
            ChatClient::instance()->AddFriend(username.toStdString());
        }
    });
}

void MainWindow::initBind()
{
    ChatClient::instance()->BindMsg([=](string msg) {
        main_task_que.push_back([=]() {
            statusBar()->showMessage(QString::fromStdString(msg), 2000);
        });
    });
    ChatClient::instance()->BindError([=](string msg) {
        main_task_que.push_back([=]() {
            statusBar()->showMessage(QString::fromStdString(msg), 2000);
        });
    });
    ChatClient::instance()->BindUsers([=](vector<user_t> users) {
        main_task_que.push_back([=]() {
            this->users = users;
            users_list_view->clear();
            for(int i = 0; i < users.size(); i ++) {
                QString text = QString::fromStdString(users[i].username);
                text += users[i].online ? "[o]" : "[x]";
                users_list_view->addItem(text);
            }
        });
    });
    ChatClient::instance()->BindFriends([=](vector<user_t> friends) {
        main_task_que.push_back([=]() {
            this->friends = friends;
            friends_list_view->clear();
            for(int i = 0; i < friends.size(); i ++) {
                QString text = QString::fromStdString(friends[i].username);
                text += friends[i].online ? "[o]" : "[x]";
                if (friends[i].unread) text += "(" + QString::number(friends[i].unread) + ")";
                friends_list_view->addItem(text);
            }
        });
    });
}