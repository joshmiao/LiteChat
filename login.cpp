#include "login.h"
#include "ui_login.h"

Login::Login(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::Login)
{
    ui->setupUi(this);
    client = new QTcpSocket();
    client->connectToHost("192.168.1.105", 1234);
    connect(client, SIGNAL(connected()), this, SLOT(handConnected()));
}

Login::~Login()
{
    delete ui;
}

void Login::handConnected()
{
    ui->pushButton->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    connect(client, SIGNAL(readyRead()), this, SLOT(handReadyRead()));
}


void Login::on_pushButton_clicked()
{
    QString uName = ui->lineEdit->text();
    QString pWord = ui->lineEdit_2->text();
    QString packData = "#1|" + uName + "|" + pWord + "&";
    client->write(packData.toLocal8Bit());
}


void Login::handReadyRead()
{
    QByteArray recvArray = client->readAll();
    if(recvArray.at(0) != '#' || recvArray.at(recvArray.length() - 1) != '&')
    {
        return;
    }
    QString recvStr = QString::fromLocal8Bit(recvArray);
    recvStr = recvStr.mid(1, recvStr.length() - 2);
    QStringList recvList = recvStr.split('|');
    if(recvList.length() < 3)
    {
        return;
    }
    if(recvList[0] == "1")
    {
        if(recvList[1] == "0")
        {
            QMessageBox::information(this, "提示", "登陆成功");
            disconnect(client,SIGNAL(readyRead()), 0, 0);
            mw = new MainWindow(client);
            mw->show();
            this->hide();
        }
        if(recvList[1] == "1")
        {
            QMessageBox::warning(this, "警告", recvList[2]);
        }
    }
    else if(recvList[0] == "2")
    {
        if(recvList[1] == "0")
        {
            QMessageBox::information(this, "提示", "注册成功");
            reg->close();
            reg = NULL;
            this ->show();
        }
        if(recvList[1] == "1")
        {
            QMessageBox::warning(this, "警告", recvList[2]);
        }
    }
}


void Login::on_pushButton_2_clicked()
{
    reg = new Register(client);
    reg->show();
    this->hide();
}










