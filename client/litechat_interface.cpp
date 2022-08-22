#include "litechat_interface.h"
#include "litechat_chatlist.h"
#include "ui_litechat_interface.h"

LiteChat_Interface::LiteChat_Interface(LiteChat *liteChatMain, QString loginName, int32_t loginId, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LiteChat_Interface),
    liteChatMain(liteChatMain),
    loginName(loginName),
    loginId(loginId)
{
    chatList = liteChatMain->createChatList();
    currentDialog = liteChatMain->createDialog("测试私聊", LiteChat_Dialog::Private, 1);
    ui->setupUi(this);
    ui->horizontalLayout->addWidget(chatList);
    ui->horizontalLayout->addWidget(currentDialog);
}

LiteChat_Interface::~LiteChat_Interface()
{
    delete ui;
}
