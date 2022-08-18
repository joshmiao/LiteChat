#include <QMessageBox>
#include "litechat_login.h"
#include "ui_litechat_login.h"


LiteChat_Login::LiteChat_Login(LiteChat *liteChatMain, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LiteChat_Login),
    liteChatMain(liteChatMain)
{
    ui->setupUi(this);
}

LiteChat_Login::~LiteChat_Login()
{
    delete ui;
}

void LiteChat_Login::on_pushButton_clicked()
{
    QMessageBox msgBox;
    if (liteChatMain->sendtoServer("Login attempt") == 0) {
        msgBox.setText("Success");
    }
    else{
        msgBox.setText("Fail");
    }
    msgBox.exec();
}

