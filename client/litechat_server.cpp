#include <QDebug>
#include "litechat_server.h"
#include "litechat_interface.h"
#include "litechat_dialog.h"
#include "litechat_login.h"
#include "litechat_register.h"
#include "ui_litechat_server.h"
#include "request_type.h"

#include <json.hpp>
using json = nlohmann::json;

UserInfo::UserInfo(int32_t id, QString username) :
    id(id),
    username(username)
{

}

LiteChat_Server::LiteChat_Server(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LiteChat_Server),
    userInfo(-1)
{
    ui->setupUi(this);
    client = new QTcpSocket();
    serverStatus = false;
    loginStatus = false;
}

LiteChat_Server::~LiteChat_Server()
{
    delete ui;
}

void LiteChat_Server::on_pushButton_clicked()
{
    QString ip = ui->lineEdit->text();
    QString port = ui->lineEdit_2->text();
    if (ip == "" || port == ""){
        ip = QString("192.168.50.249");
        port = QString("1234");
        ui->textEdit->append("Input correct IP and Port!");
//        return;
    }
    client->connectToHost(ip, port.toShort());
    ui->textEdit->append("tring to connect...");
    connect(client, SIGNAL(connected()), this, SLOT(handConnected()));
}

void LiteChat_Server::handConnected()
{
    serverStatus = true;
    ui->pushButton_2->setEnabled(true);
    ui->textEdit->append("successfully connected!");
    connect(client, SIGNAL(readyRead()), this, SLOT(handReadyRead()));
    LiteChat_Login *loginPage = createLoginPage();
    loginPage->show();
//    this->hide();
}

void LiteChat_Server::handReadyRead()
{
    QByteArray recvArray = client->readAll();
    QString recvString = QString::fromUtf8(recvArray);
    json j;
    try
    {
        /*
         {"data":{"friends":[{"user_id":66666,"user_name":"nonono1"},{"user_id":88888,"user_name":"nonono2"}],"result":"success_get_friends"},"type":1004}
        */
        j = json::parse(recvString.toStdString());
        qDebug() << "this is valid to parse:" << QString::fromStdString(to_string(j))<< '\n';
        if (j["type"] == _GET_FRIENDS){
            j = j["data"];
            if (std::string(j["result"]) != "success_get_friends") return;
            for (const auto &f : j["friends"])
            {
                int32_t id = f["user_id"];
                QString name = QString::fromStdString(std::string(f["user_name"]));
                emit newFriendRecieve(LiteChat_Dialog::Private, id, name);
            }
        }

    } catch (...)
    {
        qDebug() << "Invalid sequence received\n";
        if (recvString[0] == '#'){
            qDebug() << recvString.mid(1);
            emit messageReceive(LiteChat_Dialog::Private, 10002, "新建私聊", recvString.mid(1));
        }
        if (recvString[0] == '*'){
            loginStatus = true;
            token = recvString.mid(1);
            userInfo.id = 10001;
            userInfo.username = "test username";
            emit loginSuccess("test username", 10001);
        }
    }
    ui->textEdit->append(recvString);
}

int LiteChat_Server::sendtoServer(json j)
{
    if (!serverStatus) return -1;
    QString msg = to_string(j).c_str();
    client->write(msg.toUtf8());
    return 0;
}

void LiteChat_Server::on_pushButton_2_clicked()
{
    QString txt = ui->lineEdit_3->text();
    client->write(txt.toUtf8());
}

int LiteChat_Server::sendMessage(LiteChat_Dialog::Dialog_Type dialogType, int32_t toId, QString msg)
{
    if (!loginStatus) return -1;
    json j;
    j["type"] = dialogType == LiteChat_Dialog::Private ? _PRIVATE_MESSAGE : _GROUP_MESSAGE;
    j["token"] = token.toUtf8();
    j["data"]["user_id"] = userInfo.id;
    j["data"]["to_id"] = toId;
    j["data"]["content"] = msg.toUtf8();
    return sendtoServer(j);
}

int LiteChat_Server::requestLogin(int32_t id, QString pwd)
{
    json j;
    j["type"] = 1;
    j["data"]["user_id"] = id;
    j["data"]["pwd"] = pwd.toUtf8();
    return sendtoServer(j);
}

LiteChat_Login* LiteChat_Server::createLoginPage()
{
    LiteChat_Login *loginPage = new LiteChat_Login(this);
    connect(this, &LiteChat_Server::loginSuccess, loginPage, &LiteChat_Login::loginSuccess);
    return loginPage;
}

LiteChat_Dialog* LiteChat_Server::createDialog(QString chatName, LiteChat_Dialog::Dialog_Type dialogType, int32_t toId)
{
    LiteChat_Dialog *dialogPage = new LiteChat_Dialog(this, chatName, dialogType, toId);
//    connect(this, &LiteChat_Server::messageReceive, dialogPage, &LiteChat_Dialog::receiveSingalMessage);
    return dialogPage;
}

LiteChat_Interface* LiteChat_Server::createInterface(QString loginName, int32_t loginId){
    LiteChat_Interface *interface = new LiteChat_Interface(this, loginName, loginId);
    connect(this, &LiteChat_Server::messageReceive, interface, &LiteChat_Interface::messageReceive);
    connect(this, &LiteChat_Server::newFriendRecieve, interface, &LiteChat_Interface::addSingleDialogListItem);
    return interface;
}

LiteChat_Register* LiteChat_Server::createRegister(){
    LiteChat_Register *Register = new LiteChat_Register(this);
    return Register;
}
