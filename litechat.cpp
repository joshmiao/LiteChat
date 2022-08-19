#include <QDebug>
#include "litechat.h"
#include "ui_litechat.h"
#include "litechat_login.h"
#include "litechat_privatechat.h"

LiteChat::LiteChat(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LiteChat)
{
    ui->setupUi(this);
    client = new QTcpSocket();
    serverReady = false;
}

LiteChat::~LiteChat()
{
    delete ui;
}


void LiteChat::on_pushButton_clicked()
{
    QString ip = ui->lineEdit->text();
    QString port = ui->lineEdit_2->text();
    if (ip == "" || port == ""){
        ui->textEdit->append("Input correct IP and Port!");
        return;
    }
    client->connectToHost(ip, port.toShort());
    ui->textEdit->append("tring to connect...");
    connect(client, SIGNAL(connected()), this, SLOT(handConnected()));
}

void LiteChat::handConnected()
{
    serverReady = true;
    ui->pushButton_2->setEnabled(true);
    ui->textEdit->append("successfully connected!");
    connect(client, SIGNAL(readyRead()), this, SLOT(handReadyRead()));
}

int LiteChat::sendtoServer(QString msg)
{
    if (!serverReady) return -1;
    client->write(msg.toUtf8());
    return 0;
}

void LiteChat::on_pushButton_2_clicked()
{
    QString txt = ui->lineEdit_3->text();
    sendtoServer(txt);
}

void LiteChat::createPrivateChat()
{
    LiteChat_PrivateChat *privateChatPage = new LiteChat_PrivateChat(this);
    privateChatPage->show();
    connect(this, &LiteChat::messageReceive, privateChatPage, &LiteChat_PrivateChat::showMessage);
}

void LiteChat::createLoginPage()
{
    LiteChat_Login *loginPage = new LiteChat_Login(this);
    loginPage->show();
}

void LiteChat::handReadyRead()
{
    QByteArray recvArray = client->readAll();
    QString recvString = QString::fromUtf8(recvArray);
    if (recvString == "connect_success"){
        createLoginPage();
        this->hide();
    }
    if (recvString[0] == '#'){
        qDebug() << recvString.mid(1);
        emit messageReceive(recvString.mid(1));
    }
    ui->textEdit->append(recvString);
}
