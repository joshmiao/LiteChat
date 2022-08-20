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
    QString req = "#usr|" + ui->lineEdit->text() + "|#pwd|" + ui->lineEdit_2->text();
    QMessageBox msgBox;
    if (liteChatMain->sendtoServer(req) == 0) {
        msgBox.setText("Success");
        liteChatMain->createDialog();
        this->hide();
    }
    else{
        msgBox.setText("Fail");
    }
    msgBox.exec();
}

