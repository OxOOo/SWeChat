#include "welcome.h"
#include "chat/client.h"
#include <QDesktopWidget>
#include <QApplication>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QTimer>
#include <thread>

using namespace swechat;

WelcomeWindow::WelcomeWindow()
{
    initUI();
    done_rst = -1;

    last_msg_time = QDateTime::currentDateTime();
    msg_timer = new QTimer(this);
    connect(msg_timer, &QTimer::timeout, [=]() {
        if (last_msg_time.msecsTo(QDateTime::currentDateTime()) > 1000)
            msg_label->setText("正常");
        if (done_rst != -1) done(done_rst);
    });
    msg_timer->start(200);
}

WelcomeWindow::~WelcomeWindow()
{
}

void WelcomeWindow::initUI()
{
    setFixedSize(300, 200);
    QDesktopWidget *desktop = QApplication::desktop();
    move((desktop->width() - this->width()) / 2, (desktop->height() - this->height()) / 2);
    setWindowTitle("SWeChat");

    QFormLayout* f_layout = new QFormLayout();
    f_layout->addRow("地址：", address_edit = new QLineEdit("127.0.0.1"));
    f_layout->addRow("用户名：", username_edit = new QLineEdit("admin")); // fixme
    f_layout->addRow("密码：", password_edit = new QLineEdit("123")); // fixme
    f_layout->setSpacing(10);
    f_layout->setMargin(10);
    f_layout->setLabelAlignment(Qt::AlignRight);

    QHBoxLayout* h_layout = new QHBoxLayout();
    h_layout->addWidget(register_btn = new QPushButton("注册", this));
    h_layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum));
    h_layout->addWidget(login_btn = new QPushButton("登录", this));

    QVBoxLayout* v_layout = new QVBoxLayout();
    v_layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));
    v_layout->addLayout(f_layout);
    v_layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Minimum, QSizePolicy::Expanding));
    v_layout->addLayout(h_layout);
    v_layout->addWidget(msg_label = new QLabel(this));

    setLayout(v_layout);

    password_edit->setEchoMode(QLineEdit::Password);
    username_edit->setFocus();
    msg_label->setText("正常");
    msg_label->show();
    login_btn->setDefault(true);

    connect(login_btn, &QPushButton::clicked, [this]() {
        showMsg("登录ing");
        ChatClient::instance()->Connect(address_edit->text().toStdString(), [this](bool success) {
            if (success) {
                ChatClient::instance()->Login(username_edit->text().toStdString(), password_edit->text().toStdString(), [this](bool success) {
                    if (!success) {
                        ChatClient::instance()->Close();
                    } else {
                        done_rst = 1;
                    }
                });
            }
        });
    });
    connect(register_btn, &QPushButton::clicked, [this]() {
        showMsg("注册ing");
        ChatClient::instance()->Connect(address_edit->text().toStdString(), [this](bool success) {
            if (success) {
                ChatClient::instance()->Register(username_edit->text().toStdString(), password_edit->text().toStdString(), [this](bool success) {
                    ChatClient::instance()->Close();
                    showMsg("注册成功，可以登录");
                });
            }
        });
    });
    ChatClient::instance()->BindMsg([this](string msg) {
        showMsg(msg.c_str());
    });
    ChatClient::instance()->BindError([this](string msg) {
        showMsg(msg.c_str());
    });
}

void WelcomeWindow::showMsg(QString msg)
{
    msg_label->setText(msg);
    last_msg_time = QDateTime::currentDateTime();
}