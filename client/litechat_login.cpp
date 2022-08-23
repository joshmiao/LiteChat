#include <QMessageBox>
#include "litechat_login.h"
#include "litechat_register.h"
#include "litechat_interface.h"
#include "ui_litechat_login.h"

#include <json.hpp>
using json = nlohmann::json;

LiteChat_Login::LiteChat_Login(LiteChat_Server *liteChatServer, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LiteChat_Login),
    liteChatServer(liteChatServer)
{
    ui->setupUi(this);
}

LiteChat_Login::~LiteChat_Login()
{
    delete ui;
}

void LiteChat_Login::on_pushButton_clicked()
{
    liteChatServer->requestLogin(ui->lineEdit->text().toInt(), ui->lineEdit_2->text());
}

void LiteChat_Login::on_pushButton_2_clicked()
{
    LiteChat_Register *Register = liteChatServer->createRegister();
    Register->show();
    delete this;
}

void LiteChat_Login::loginSuccess(QString loginName, int32_t loginId){
    LiteChat_Interface* interface = liteChatServer->createInterface(loginName, loginId);
    interface->show();
    QMessageBox msgBox;
    msgBox.setText("登录成功！欢迎您" + loginName);
    msgBox.exec();
    delete this;
}

