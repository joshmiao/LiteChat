#ifndef LITECHAT_INVITEFRIEND_H
#define LITECHAT_INVITEFRIEND_H

#include <QWidget>

namespace Ui {
class LiteChat_InviteFriend;
}

class LiteChat_InviteFriend : public QWidget
{
    Q_OBJECT

public:
    explicit LiteChat_InviteFriend(QWidget *parent = nullptr);
    ~LiteChat_InviteFriend();

private:
    Ui::LiteChat_InviteFriend *ui;
};

#endif // LITECHAT_INVITEFRIEND_H
