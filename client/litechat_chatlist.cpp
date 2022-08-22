#include "litechat_chatlist.h"
#include "ui_litechat_chatlist.h"

LiteChat_ChatList::LiteChat_ChatList(LiteChatServer *liteChatServer, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LiteChat_ChatList),
    liteChatServer(liteChatServer)
{
    ui->setupUi(this);
}

LiteChat_ChatList::~LiteChat_ChatList()
{
    delete ui;
}


LiteChat_ChatListItem::LiteChat_ChatListItem(QWidget *parent) :
    QWidget(parent)
{

}

LiteChat_ChatListItem::~LiteChat_ChatListItem()
{

}
