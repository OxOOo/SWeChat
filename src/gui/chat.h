#ifndef GUI_CHAT_H
#define GUI_CHAT_H

#include <QString>
#include <QWidget>
#include "chat/client.h"

namespace swechat
{
    using namespace std;

    class ChatWidget : public QWidget
    {
    public:
        ChatWidget(QString target_user, QWidget* parent = NULL);
        ~ChatWidget();
    
    private:
        void initUI();
        
        QString target_user;
    };
}

#endif // GUI_CHAT_H