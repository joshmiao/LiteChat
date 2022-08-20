#include "litechat_chatlist.h"
#include "ui_litechat_chatlist.h"

LiteChat_ChatList::LiteChat_ChatList(LiteChat *liteChatMain, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LiteChat_ChatList),
    liteChatMain(liteChatMain)
{
    ui->setupUi(this);
}

LiteChat_ChatList::~LiteChat_ChatList()
{
    delete ui;
}
