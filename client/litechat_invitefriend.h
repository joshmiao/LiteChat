#ifndef LITECHAT_INVITEFRIEND_H
#define LITECHAT_INVITEFRIEND_H

#include <QWidget>
#include <QLabel>

namespace Ui {
class LiteChat_InviteFriend;
}

class LiteChat_Server;
class DialogInfo;

class LiteChat_FriendListItem : public QWidget
{
    Q_OBJECT
public:
    explicit LiteChat_FriendListItem(int32_t id, QString name, QWidget *parent = nullptr);
    ~LiteChat_FriendListItem() = default;
    int32_t id;
    QString name;

private:
    QLabel *NameLabel, *IdLabel;
};


class LiteChat_InviteFriend : public QWidget
{
    Q_OBJECT

public:
    explicit LiteChat_InviteFriend(LiteChat_Server *liteChatServer, int32_t groupId, std::vector<DialogInfo>, QWidget *parent = nullptr);
    ~LiteChat_InviteFriend();

private:
    Ui::LiteChat_InviteFriend *ui;
    LiteChat_Server *liteChatServer;
    int32_t groupId;
    std::vector<DialogInfo> dialogInfo;

private slots:
    void inviteUser(int currentRow);
};

#endif // LITECHAT_INVITEFRIEND_H
