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

using namespace swechat;

MainWindow::MainWindow()
{
    initUI();
    initBind();

    ChatClient::instance()->RecvLoop();
    ChatClient::instance()->Flash();
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
    right_layout->addWidget(name_label = new QLabel("我的名字"));
    // right_layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));

    QTabWidget* tab_widget = new QTabWidget(this);
    tab_widget->addTab(friends_list_view = new QListWidget(this), "我的好友");
    tab_widget->addTab(users_list_view = new QListWidget(this), "所有用户");
    right_layout->addWidget(tab_widget);

    QSizePolicy tab_size_policy = tab_widget->sizePolicy();
    tab_size_policy.setVerticalPolicy(QSizePolicy::Expanding);
    tab_widget->setSizePolicy(tab_size_policy);

    QHBoxLayout* main_layout = new QHBoxLayout();
    main_layout->addWidget(chat = new ChatWidget("未知", this), 3);
    // main_layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum));
    main_layout->addLayout(right_layout, 1);

    QWidget* central_widget = new QWidget(this);
    setCentralWidget(central_widget);

    central_widget->setLayout(main_layout);

    connect(friends_list_view, &QListWidget::itemDoubleClicked, [=](QListWidgetItem * item) {
        qDebug() << item->text() << " " << friends_list_view->currentRow();
    });
    connect(users_list_view, &QListWidget::itemDoubleClicked, [=](QListWidgetItem * item) {
        qDebug() << item->text();
    });

    friends_list_view->addItem("好友1");
    friends_list_view->addItem("好友2");
    friends_list_view->addItem("好友3");

    users_list_view->addItem("用户1");
    users_list_view->addItem("用户2");
    users_list_view->addItem("用户3");
}

void MainWindow::initBind()
{
    ChatClient::instance()->BindMsg([=](string msg) {
        statusBar()->showMessage(QString::fromStdString(msg));
    });
    ChatClient::instance()->BindError([=](string msg) {
        statusBar()->showMessage(QString::fromStdString(msg));
    });
    ChatClient::instance()->BindUsers([=](vector<user_t> users) {
        this->users = users;
        users_list_view->clear();
        for(int i = 0; i < users.size(); i ++) {
            QString text = QString::fromStdString(users[i].username);
            text += users[i].online ? "[o]" : "[x]";
            users_list_view->addItem(text);
        }
    });
}