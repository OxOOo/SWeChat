#ifndef GUI_WELCOME_H
#define GUI_WELCOME_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>

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

        QLineEdit* address_edit;
        QLineEdit* username_edit;
        QLineEdit* password_edit;
        QPushButton* login_btn;
        QPushButton* register_btn;
    };
}

#endif // GUI_WELCOME_H