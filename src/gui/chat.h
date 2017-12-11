#ifndef GUI_CHAT_H
#define GUI_CHAT_H

#include "chat/client.h"
#include <functional>
#include <QString>
#include <QWidget>
#include <QTextBrowser>
#include <QTextEdit>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QQueue>

namespace swechat
{
    using namespace std;

    class ChatWidget : public QWidget
    {
    public:
        ChatWidget(QWidget* parent = NULL);
        ~ChatWidget();

        void setUser(QString username);
    
    private:
        void appendMesssage(message_t message);

        void initUI();
        void initBind();

        QTimer* timer;
        QQueue<function<void()> > main_task_que;
        
        QString username;
        QLabel* title_label;
        QTextBrowser* msg_browser;
        QTextEdit* msg_edit;
        QPushButton* send_btn;
        QPushButton* file_btn;
    };
}

#endif // GUI_CHAT_H