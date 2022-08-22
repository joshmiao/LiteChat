#include "litechat_interface.h"
#include "litechat_chatlist.h"
#include "ui_litechat_interface.h"

LiteChat_Interface::LiteChat_Interface(LiteChatServer *liteChatServer, QString loginName, int32_t loginId, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LiteChat_Interface),
    liteChatServer(liteChatServer),
    userinfo(loginId, loginName)
{
    chatList = liteChatServer->createChatList();
    currentDialog = liteChatServer->createDialog("测试私聊", LiteChat_Dialog::Private, 10002);
    ui->setupUi(this);
    ui->horizontalLayout->addWidget(chatList);
    ui->horizontalLayout->addWidget(currentDialog);
}

LiteChat_Interface::~LiteChat_Interface()
{
    delete ui;
}
