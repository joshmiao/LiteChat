#include <QDebug>
#include "litechat.h"
#include "litechat_interface.h"
#include "litechat_dialog.h"
#include "litechat_login.h"
#include "litechat_register.h"
#include "litechat_chatlist.h"
#include "ui_litechat.h"

#include <json.hpp>
using json = nlohmann::json;

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
        ip = QString("192.168.1.105");
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
    if (recvString[0] == '*'){
        emit loginSuccess("testname", 10001);
    }
    ui->textEdit->append(recvString);
}

int LiteChat::sendtoServer(json j)
{
    QString msg = to_string(j).c_str();
    if (!serverReady) return -1;
    client->write(msg.toUtf8());
    return 0;
}

void LiteChat::on_pushButton_2_clicked()
{
    QString txt = ui->lineEdit_3->text();
    client->write(txt.toUtf8());
}

int LiteChat::sendMessage(LiteChat_Dialog::Dialog_Type dialogType, int32_t id, QString msg)
{
    json j;
    j["type"] = 2;
    j["token"] = token.toUtf8();
    j["content"]["dialogType"] = dialogType;
    j["content"]["id"] = id;
    j["content"]["msg"] = msg.toUtf8();
    return sendtoServer(j);
}

int LiteChat::requestLogin(int32_t id, QString pwd)
{
    json j;
    j["type"] = 1;
    j["content"]["id"] = id;
    j["content"]["pwd"] = pwd.toUtf8();
    return sendtoServer(j);
}

LiteChat_Login* LiteChat::createLoginPage()
{
    LiteChat_Login *loginPage = new LiteChat_Login(this);
    connect(this, &LiteChat::loginSuccess, loginPage, &LiteChat_Login::loginSuccess);
    return loginPage;
}

LiteChat_Dialog* LiteChat::createDialog(QString chatName, LiteChat_Dialog::Dialog_Type dialogType, int id)
{
    LiteChat_Dialog *dialogPage = new LiteChat_Dialog(this, chatName, dialogType, id);
    connect(this, &LiteChat::messageReceive, dialogPage, &LiteChat_Dialog::receiveSingalMessage);
    return dialogPage;
}

LiteChat_ChatList* LiteChat::createChatList(){
    LiteChat_ChatList *chatList = new LiteChat_ChatList(this);
    return chatList;
}

LiteChat_Interface* LiteChat::createInterface(QString loginName, int32_t loginId){
    LiteChat_Interface *interface = new LiteChat_Interface(this, loginName, loginId);
    return interface;
}

LiteChat_Register* LiteChat::createRegister(){
    LiteChat_Register *Register = new LiteChat_Register(this);
    return Register;
}
