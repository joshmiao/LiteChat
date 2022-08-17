#include "litechat.h"
#include "ui_litechat.h"

LiteChat::LiteChat(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LiteChat)
{
    ui->setupUi(this);
}

LiteChat::~LiteChat()
{
    delete ui;
}

