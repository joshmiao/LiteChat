#ifndef LITECHAT_CHATLIST_H
#define LITECHAT_CHATLIST_H

#include <QWidget>
#include "litechat.h"

namespace Ui {
class LiteChat_ChatList;
}

class LiteChat_ChatList : public QWidget
{
    Q_OBJECT

public:
    explicit LiteChat_ChatList(LiteChat *liteChatMain, QWidget *parent = nullptr);
    ~LiteChat_ChatList();

private:
    Ui::LiteChat_ChatList *ui;
    LiteChat *liteChatMain;
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
