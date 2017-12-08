#ifndef GUI_WELCOME_H
#define GUI_WELCOME_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <QDateTime>
#include <QEventLoop>

namespace swechat
{

    class WelcomeWindow : public QDialog
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
        int done_rst;
    };
}

#endif // GUI_WELCOME_H