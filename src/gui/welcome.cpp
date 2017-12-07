#include "welcome.h"
#include <QDesktopWidget>
#include <QApplication>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>

using namespace swechat;

WelcomeWindow::WelcomeWindow()
{
    initUI();
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
    f_layout->addRow("用户名：", username_edit = new QLineEdit());
    f_layout->addRow("密码：", password_edit = new QLineEdit());
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

    setLayout(v_layout);

    password_edit->setEchoMode(QLineEdit::Password);
    username_edit->setFocus();
}