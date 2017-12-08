#ifndef GUI_WELCOME_H
#define GUI_WELCOME_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QDateTime>

namespace swechat
{

    class WelcomeWindow : public QWidget
    {
        Q_OBJECT
    public:
        WelcomeWindow();
        ~WelcomeWindow();

    private:
        void initUI();
        void showMsg(QString msg);

        QLineEdit* address_edit;
        QLineEdit* username_edit;
        QLineEdit* password_edit;
        QPushButton* login_btn;
        QPushButton* register_btn;
        QLabel* msg_label;
        QTimer* msg_timer;
        QDateTime last_msg_time;
    };
}

#endif // GUI_WELCOME_H