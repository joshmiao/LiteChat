#include <QDebug>
#include "litechat.h"
#include "litechat_interface.h"
#include "litechat_dialog.h"
#include "litechat_login.h"
#include "ui_litechat.h"


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
        ip = QString("192.168.1.3");
        port = QString("1234");
        ui->textEdit->append("Input correct IP and Port!");
//        return;
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

void LiteChat::handReadyRead()
{
    QByteArray recvArray = client->readAll();
    QString recvString = QString::fromUtf8(recvArray);
    if (recvString == "succ"){
        LiteChat_Login *loginPage = createLoginPage();
        loginPage->show();
        this->hide();
    }
    if (recvString[0] == '#'){
        qDebug() << recvString.mid(1);
        emit messageReceive(recvString.mid(1));
    }
    ui->textEdit->append(recvString);
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

LiteChat_Login* LiteChat::createLoginPage()
{
    LiteChat_Login *loginPage = new LiteChat_Login(this);
    return loginPage;
}

LiteChat_Dialog* LiteChat::createDialog()
{
    LiteChat_Dialog *dialogPage = new LiteChat_Dialog(this, "Test Dialog", LiteChat_Dialog::Private);
    connect(this, &LiteChat::messageReceive, dialogPage, &LiteChat_Dialog::receiveSingalMessage);
    return dialogPage;
}

LiteChat_ChatList* LiteChat::createChatList(){
    LiteChat_ChatList *chatList = new LiteChat_ChatList(this);
    return chatList;
}

LiteChat_Interface* LiteChat::createInterface(){
    LiteChat_Interface *interface = new LiteChat_Interface(this);
    return interface;
}
