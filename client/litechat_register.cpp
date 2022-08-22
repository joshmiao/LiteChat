#include "litechat_register.h"
#include "ui_litechat_register.h"

LiteChat_Register::LiteChat_Register(LiteChatServer *liteChatServer ,QWidget *parent) :
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
