#include "litechat_register.h"
#include "ui_litechat_register.h"

LiteChat_Register::LiteChat_Register(LiteChat_Server *liteChatServer ,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LiteChat_Register),
    liteChatServer(liteChatServer)
{
    ui->setupUi(this);
}

LiteChat_Register::~LiteChat_Register()
{
    delete ui;
}
