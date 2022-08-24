#ifndef LITECHAT_INVITEFRIEND_H
#define LITECHAT_INVITEFRIEND_H

#include <QWidget>
#include <QLabel>

#include <litechat_dialog.h>

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
    explicit LiteChat_InviteFriend(LiteChat_Server *liteChatServer, int32_t groupId, QWidget *parent = nullptr);
    ~LiteChat_InviteFriend();

public slots:
    void addSingleFriend(LiteChat_Dialog::Dialog_Type type, int32_t id, QString name);

private:
    Ui::LiteChat_InviteFriend *ui;
    LiteChat_Server *liteChatServer;
    int32_t groupId;
    std::map<uint32_t, std::pair<int32_t, QString>> friendListIndex;

private slots:
    void inviteUser(int currentRow);
};

#endif // LITECHAT_INVITEFRIEND_H
