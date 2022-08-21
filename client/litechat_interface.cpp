#include "litechat_interface.h"
#include "ui_litechat_interface.h"

LiteChat_Interface::LiteChat_Interface(LiteChat *liteChatMain, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LiteChat_Interface),
    liteChatMain(liteChatMain)
{
    chatList = liteChatMain->createChatList();
    currentDialog = liteChatMain->createDialog();
    ui->setupUi(this);
    ui->horizontalLayout->addWidget(chatList);
    ui->horizontalLayout->addWidget(currentDialog);
}

LiteChat_Interface::~LiteChat_Interface()
{
    delete ui;
}
