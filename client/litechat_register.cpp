#include <QMessageBox>
#include "litechat_server.h"
#include "litechat_login.h"
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

void LiteChat_Register::on_pushButton_clicked()
{
    if (ui->lineEdit_2->text() != ui->lineEdit_3->text()){
        QMessageBox msg;
        msg.setIcon(QMessageBox::Warning);
        msg.setText("您两次输入的密码不一样！请重新输入");
        msg.exec();
        ui->lineEdit_2->setText("");
        ui->lineEdit_3->setText("");
        return;
    }
    liteChatServer->requestRegister(ui->lineEdit->text(), ui->lineEdit_2->text(), ui->lineEdit_4->text());
}

void LiteChat_Register::registerSuccess(int32_t id)
{
    QMessageBox msg;
    msg.setIcon(QMessageBox::Information);
    msg.setText("注册成功!\n请记住您的ID: " + QString::fromStdString(std::to_string(id)));
    msg.exec();
    LiteChat_Login *loginPage = liteChatServer->createLoginPage();
    loginPage->show();
    delete this;
}
