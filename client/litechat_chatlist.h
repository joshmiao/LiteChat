#ifndef LITECHAT_CHATLIST_H
#define LITECHAT_CHATLIST_H

#include <QWidget>

namespace Ui {
class LiteChat_ChatList;
}

class LiteChatServer;

class LiteChat_ChatList : public QWidget
{
    Q_OBJECT

public:
    explicit LiteChat_ChatList(LiteChatServer *liteChatServer, QWidget *parent = nullptr);
    ~LiteChat_ChatList();

private:
    Ui::LiteChat_ChatList *ui;
    LiteChatServer *liteChatServer;
};

class LiteChat_ChatListItem : public QWidget
{
    Q_OBJECT

public:
    explicit LiteChat_ChatListItem(QWidget *parent = nullptr);
    ~LiteChat_ChatListItem();

private:


};

#endif // LITECHAT_CHATLIST_H
