#include "litechat_register.h"
#include "ui_litechat_register.h"

LiteChat_Register::LiteChat_Register(LiteChat *liteChatMain ,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LiteChat_Register),
    liteChatMain(liteChatMain)
{
    ui->setupUi(this);
}

LiteChat_Register::~LiteChat_Register()
{
    delete ui;
}
