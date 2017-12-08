#include "chat.h"
#include <QLabel>

using namespace swechat;

ChatWidget::ChatWidget(QString target_user, QWidget* parent)
    : QWidget(parent), target_user(target_user)
{
    initUI();
}

ChatWidget::~ChatWidget()
{

}

void ChatWidget::initUI()
{
    QSizePolicy size_policy = sizePolicy();
    size_policy.setHorizontalPolicy(QSizePolicy::Expanding);
    setSizePolicy(size_policy);

    QLabel* label = new QLabel(this);
    label->setText(target_user);
}
