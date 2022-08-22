#include <QMessageBox>
#include "litechat_login.h"
#include "litechat_register.h"
#include "litechat_interface.h"
#include "ui_litechat_login.h"

#include <json.hpp>
using json = nlohmann::json;

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
    liteChatMain->requestLogin(ui->lineEdit->text().toInt(), ui->lineEdit_2->text());
}

void LiteChat_Login::on_pushButton_2_clicked()
{
    LiteChat_Register *Register = liteChatMain->createRegister();
    Register->show();
    this->hide();
}

void LiteChat_Login::loginSuccess(QString loginName, int32_t loginId){
    LiteChat_Interface* interface = liteChatMain->createInterface(loginName, loginId);
    interface->show();
    this->hide();
    QMessageBox msgBox;
    msgBox.setText("Success");
    msgBox.exec();
}

