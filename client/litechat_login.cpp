#include <QMessageBox>
#include <QMouseEvent>
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
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setFixedSize(this->width(),this->height());
}

LiteChat_Login::~LiteChat_Login()
{
    delete ui;
}

void LiteChat_Login::on_pushButton_clicked()
{
    if (ui->lineEdit->text() == "" || ui->lineEdit_2->text() == ""){
        QMessageBox msg;
        msg.setIcon(QMessageBox::Warning);
        msg.setText("用户名或密码为空！请重新输入");
        msg.exec();
        return;
    }
    liteChatServer->requestLogin(ui->lineEdit->text(), ui->lineEdit_2->text());
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
    msgBox.setText("登录成功！\n欢迎您 " + loginName + "!");
//    msgBox.exec();
    delete this;
}

void LiteChat_Login::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        {
            mousePress = true;
        }
        movePoint = event->globalPos() - pos();
}

void LiteChat_Login::mouseMoveEvent(QMouseEvent *event)
{
    if(mousePress)
    {
        QPoint movePos = event->globalPos();
        move(movePos - movePoint);
    }
}

void LiteChat_Login::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    mousePress = false;
}

void LiteChat_Login::on_pushButton_4_clicked()
{
    this->close();
}


void LiteChat_Login::on_pushButton_5_clicked()
{
    this->showMinimized();
}


void LiteChat_Login::on_pushButton_5_pressed()
{
    ui->pushButton_5->setStyleSheet("border-radius:10px;background-color: rgb(240, 240, 240);border-image: url(:/img/minus.png);");
}


void LiteChat_Login::on_pushButton_5_released()
{
    ui->pushButton_5->setStyleSheet("border-radius:10px;background-color: rgb(255, 255, 255);border-image: url(:/img/minus.png);");
}


void LiteChat_Login::on_pushButton_4_pressed()
{
    ui->pushButton_4->setStyleSheet("border-radius:10px;background-color: rgb(240, 240, 240);border-image: url(:/img/close.png);");
}


void LiteChat_Login::on_pushButton_4_released()
{
    ui->pushButton_4->setStyleSheet("border-radius:10px;background-color: rgb(255, 255, 255);border-image: url(:/img/close.png);");
}

