#include <QDebug>
#include "litechat_server.h"
#include "litechat_interface.h"
#include "litechat_dialog.h"
#include "litechat_login.h"
#include "litechat_register.h"
#include "litechat_chatlist.h"
#include "ui_litechat.h"

#include <json.hpp>
using json = nlohmann::json;

UserInfo::UserInfo(int32_t id, QString username) :
    id(id),
    username(username)
{

}

LiteChatServer::LiteChatServer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LiteChat),
    userInfo(-1)
{
    ui->setupUi(this);
    client = new QTcpSocket();
    serverStatus = false;
    loginStatus = false;
}

LiteChatServer::~LiteChatServer()
{
    delete ui;
}

void LiteChatServer::on_pushButton_clicked()
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

void LiteChatServer::handConnected()
{
    serverStatus = true;
    ui->pushButton_2->setEnabled(true);
    ui->textEdit->append("successfully connected!");
    connect(client, SIGNAL(readyRead()), this, SLOT(handReadyRead()));
}

void LiteChatServer::handReadyRead()
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
        loginStatus = true;
        token = recvString.mid(1);
        userInfo.id = 10001;
        userInfo.username = "test username";
        emit loginSuccess("test username", 10001);
    }
    ui->textEdit->append(recvString);
}

int LiteChatServer::sendtoServer(json j)
{
    if (!serverStatus) return -1;
    QString msg = to_string(j).c_str();
    client->write(msg.toUtf8());
    return 0;
}

void LiteChatServer::on_pushButton_2_clicked()
{
    QString txt = ui->lineEdit_3->text();
    client->write(txt.toUtf8());
}

int LiteChatServer::sendMessage(LiteChat_Dialog::Dialog_Type dialogType, int32_t toId, QString msg)
{
    if (!loginStatus) return -1;
    json j;
    j["type"] = 2;
    j["token"] = token.toUtf8();
    j["userId"] = userInfo.id;
    j["content"]["dialogType"] = dialogType;
    j["content"]["toId"] = toId;
    j["content"]["msg"] = msg.toUtf8();
    return sendtoServer(j);
}

int LiteChatServer::requestLogin(int32_t id, QString pwd)
{
    json j;
    j["type"] = 1;
    j["content"]["id"] = id;
    j["content"]["pwd"] = pwd.toUtf8();
    return sendtoServer(j);
}

LiteChat_Login* LiteChatServer::createLoginPage()
{
    LiteChat_Login *loginPage = new LiteChat_Login(this);
    connect(this, &LiteChatServer::loginSuccess, loginPage, &LiteChat_Login::loginSuccess);
    return loginPage;
}

LiteChat_Dialog* LiteChatServer::createDialog(QString chatName, LiteChat_Dialog::Dialog_Type dialogType, int32_t toId)
{
    LiteChat_Dialog *dialogPage = new LiteChat_Dialog(this, chatName, dialogType, toId);
    connect(this, &LiteChatServer::messageReceive, dialogPage, &LiteChat_Dialog::receiveSingalMessage);
    return dialogPage;
}

LiteChat_ChatList* LiteChatServer::createChatList(){
    LiteChat_ChatList *chatList = new LiteChat_ChatList(this);
    return chatList;
}

LiteChat_Interface* LiteChatServer::createInterface(QString loginName, int32_t loginId){
    LiteChat_Interface *interface = new LiteChat_Interface(this, loginName, loginId);
    return interface;
}

LiteChat_Register* LiteChatServer::createRegister(){
    LiteChat_Register *Register = new LiteChat_Register(this);
    return Register;
}
