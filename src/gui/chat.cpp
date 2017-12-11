#include "chat.h"
#include "chat/client.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QMessageBox>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QProgressDialog>

using namespace swechat;

ChatWidget::ChatWidget(QWidget* parent)
    : QWidget(parent)
{
    initUI();
    initBind();
    setEnabled(false);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [=]() {
        while(!main_task_que.isEmpty()) {
            auto task = main_task_que.head();
            main_task_que.pop_front();
            task();
        }
    });
    timer->start(200);
}

ChatWidget::~ChatWidget()
{

}

void ChatWidget::setUser(QString username)
{
    this->username = username;
    title_label->setText("与[ " + username + " ]对话中");
    ChatClient::instance()->QueryMsgs(username.toStdString());
}

void ChatWidget::appendMesssage(message_t message)
{
    msg_browser->setTextColor(Qt::blue);
    msg_browser->setCurrentFont(QFont("宋体", 12));
    msg_browser->append(QString::fromStdString("[ "+message.sender+" ] "+message.datetime));
    msg_browser->setTextColor(Qt::black);
    msg_browser->append(QString::fromStdString(message.msg));
}

void ChatWidget::initUI()
{
    QSizePolicy size_policy = sizePolicy();
    size_policy.setHorizontalPolicy(QSizePolicy::Expanding);
    setSizePolicy(size_policy);

    title_label = new QLabel("无对话人", this);
    msg_browser = new QTextBrowser(this);
    msg_edit = new QTextEdit(this);
    send_btn = new QPushButton("发送", this);
    file_btn = new QPushButton("传输文件", this);

    msg_browser->setReadOnly(true);

    QHBoxLayout* btn_layout = new QHBoxLayout();
    btn_layout->addWidget(file_btn);
    btn_layout->addSpacerItem(new QSpacerItem(10, 10, QSizePolicy::Expanding, QSizePolicy::Minimum));
    btn_layout->addWidget(send_btn);

    QVBoxLayout* main_layout = new QVBoxLayout();
    main_layout->addWidget(title_label);
    main_layout->addWidget(msg_browser, 3);
    main_layout->addWidget(msg_edit, 1);
    main_layout->addLayout(btn_layout);

    setLayout(main_layout);
}

void ChatWidget::initBind()
{
    connect(file_btn, &QPushButton::clicked, [=]() {
        QString filepath = QFileDialog::getOpenFileName(this, "传输文件");
        if (filepath == "") return;
        QString filename = QFileInfo(filepath).fileName();
        QMessageBox* m = new QMessageBox(this);
        m->setText("等待对方确认");
        m->setWindowTitle("传输文件");
        m->setModal(true);
        m->show();
        ChatClient::instance()->SendFile(username.toStdString(), filename.toStdString(), [=](bool success) {
            m->deleteLater();
            if (success)
            {
                main_task_que.push_back([=]() {
                    QMessageBox* p = new QMessageBox(this);
                    p->setText("文件传输中");
                    p->setWindowTitle("文件传输中");
                    p->setModal(true);
                    p->show();
                    ChatClient::instance()->StartSendFile(filepath.toStdString(), [=](int sent, int total) {
                    }, [=]() {
                        main_task_que.push_back([=]() {
                            p->deleteLater();
                            QMessageBox::information(this, "文件传输完成", "文件传输完成");
                        });
                    });
                });
            }
        });
    });
    connect(send_btn, &QPushButton::clicked, [=]() {
        QString msg = msg_edit->toPlainText();
        if (msg.length() == 0) {
            QMessageBox::information(this, "不能发送空消息", "不能发送空消息");
        } else {
            ChatClient::instance()->SendMsg(username.toStdString(), msg.toStdString());
        }

        msg_edit->clear();
        msg_edit->setFocus();
    });
    ChatClient::instance()->BindChatMsg([=](string username, message_t message) {
        main_task_que.push_back([=]() {
            if (username == this->username.toStdString()) {
                appendMesssage(message);
                if (message.sender == username)
                    ChatClient::instance()->AcceptMsg(username, message.id);
            } else {
                if (message.sender == username)
                    ChatClient::instance()->RejectMsg(username, message.id);
            }
        });
    });
    ChatClient::instance()->BindChatMsgs([=](string username, vector<message_t> messages) {
        main_task_que.push_back([=]() {
            if (username == this->username.toStdString()) {
                msg_browser->clear();
                for(int i = 0; i < messages.size(); i ++)
                    appendMesssage(messages[i]);
            }
        });
    });
    ChatClient::instance()->BindFile([=](string username, string filename, auto accept_cb) {
        main_task_que.push_back([=]() {
            if (QMessageBox::question(this,
                "传输文件",
                QString::fromStdString("来自[ "+username+" ]对' "+filename+" '的文件传输请求，是否接收？"),
                QMessageBox::Yes|QMessageBox::No, QMessageBox::Yes) == QMessageBox::Yes) {
                QString filepath = QFileDialog::getSaveFileName(this, "传输文件");
                if (filepath == "") {
                    accept_cb(false);
                    return;
                }
                accept_cb(true);
                main_task_que.push_back([=]() {
                    QMessageBox* p = new QMessageBox(this);
                    p->setText("文件传输中");
                    p->setWindowTitle("文件传输中");
                    p->setModal(true);
                    p->show();
                    ChatClient::instance()->StartRecvFile(filepath.toStdString(), [=](int sent, int total) {
                    }, [=]() {
                        main_task_que.push_back([=]() {
                            p->deleteLater();
                            QMessageBox::information(this, "文件传输完成", "文件传输完成");
                        });
                    });
                });
            } else {
                accept_cb(false);
            }
        });
    });
}