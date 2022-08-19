#include "litechat_privatechat.h"
#include "ui_litechat_privatechat.h"

LiteChat_PrivateChat::LiteChat_PrivateChat(LiteChat *liteChatMain, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LiteChat_PrivateChat),
    liteChatMain(liteChatMain)
{
    ui->setupUi(this);
}

LiteChat_PrivateChat::~LiteChat_PrivateChat()
{
    delete ui;
}

void LiteChat_PrivateChat::on_pushButton_clicked()
{
    QString txt = ui->textEdit->toPlainText();
    liteChatMain->sendtoServer(txt);
}

void LiteChat_PrivateChat::showMessage(QString msg)
{
    ui->textBrowser->append(msg);
}

