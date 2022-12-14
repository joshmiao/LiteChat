#include <QMessageBox>
#include <QMouseEvent>
#include <QCryptographicHash>
#include "litechat_server.h"
#include "litechat_login.h"
#include "litechat_register.h"
#include "ui_litechat_register.h"

#include <regex>

LiteChat_Register::LiteChat_Register(LiteChat_Server *liteChatServer ,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LiteChat_Register),
    liteChatServer(liteChatServer)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::Window |Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setFixedSize(this->width(),this->height());
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
    std::regex pattern("([0-9A-Za-z\\-_\\.]+)@([0-9a-z]+\\.[a-z]{2,3}(\\.[a-z]{2})?)");
    if (!std::regex_match(ui->lineEdit_4->text().toStdString(), pattern)){
        QMessageBox msg;
        msg.setIcon(QMessageBox::Warning);
        msg.setText("您输入的邮箱不合法！请重新输入");
        msg.exec();
        ui->lineEdit_4->setText("");
        return;
    }
    QCryptographicHash Hash(QCryptographicHash::Sha1);
    QString word = ui->lineEdit_2->text();
    Hash.addData(word.toLatin1().data());
    word = Hash.result().toHex().toUpper();
    liteChatServer->requestRegister(ui->lineEdit->text(), word.left(30), ui->lineEdit_4->text());
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

void LiteChat_Register::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        {
            mousePress = true;
        }
        movePoint = event->globalPos() - pos();
}

void LiteChat_Register::mouseMoveEvent(QMouseEvent *event)
{
    if(mousePress)
    {
        QPoint movePos = event->globalPos();
        move(movePos - movePoint);
    }
}

void LiteChat_Register::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    mousePress = false;
}
void LiteChat_Register::on_pushButton_4_clicked()
{
    this->close();
}


void LiteChat_Register::on_pushButton_3_clicked()
{
    LiteChat_Login *login = liteChatServer->createLoginPage();
    login->show();
    delete this;
}


void LiteChat_Register::on_pushButton_2_clicked()
{
    this->showMinimized();
}


void LiteChat_Register::on_pushButton_2_pressed()
{
    ui->pushButton_2->setStyleSheet("border-radius:10px;background-color: rgb(240, 240, 240);border-image: url(:/img/minus.png);");
}


void LiteChat_Register::on_pushButton_2_released()
{
    ui->pushButton_2->setStyleSheet("border-radius:10px;background-color: rgb(255, 255, 255);border-image: url(:/img/minus.png);");
}


void LiteChat_Register::on_pushButton_4_pressed()
{
    ui->pushButton_4->setStyleSheet("border-radius:10px;background-color: rgb(240, 240, 240);border-image: url(:/img/close.png);");
}


void LiteChat_Register::on_pushButton_4_released()
{
    ui->pushButton_4->setStyleSheet("border-radius:10px;background-color: rgb(240, 240, 240);border-image: url(:/img/close.png);");
}

