#include <QDebug>
#include <QDateTime>
#include <QMessageBox>
#include "litechat_server.h"
#include "litechat_interface.h"
#include "litechat_dialog.h"
#include "litechat_login.h"
#include "litechat_register.h"
#include "litechat_finduser.h"
#include "litechat_invitefriend.h"
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
    if (recvString[0] == '*'){
        userInfo.id = 9999;
        userInfo.username = "test_user";
        token = "test_token";
        loginStatus = true;
        emit loginSuccess(userInfo.username, userInfo.id);
    }
    int begin = 0;
    while (true){
        int k = begin + 1, seq = 1;
        for(; seq && k < recvString.length(); k++)
        if(recvString[k] == '{') seq++;
        else if(recvString[k] == '}')seq--;

        QString str=recvString.mid(begin,k-begin);
        settleJson(str);

        if(k==recvString.length())break;
        else begin=k;
    }

    ui->textEdit->append(recvString);
}

void LiteChat_Server::settleJson(QString str)
{
    try
    {
        json j;
        /*
         * {"data":[{"friend_id":66666,"friend_name":"nonono1"},{"friend_id":88888,"friend_name":"nonono2"}],"result":"success_get_friends","type":1004}{"data":{"group_description":"This is a group created by test_user","group_name":"缇よ亰鍚嶇О","group_id":9999},"token":"test_token","type":1014}
         *
         *
         */
        j = json::parse(str.toStdString());
        qDebug() << "this is valid to parse:" << QString::fromStdString(to_string(j))<< '\n';
        if (!j["type"].is_number_integer()) throw std::runtime_error("the format is invalid!");
        if (!j["data"].is_array())
        if (j["data"]["result"] != j["null"] && j["data"]["result"] == "failed"){
            QString msg = "未知错误";
            if (j["data"]["error"] != j["null"]) msg = QString::fromStdString(std::string(j["data"]["error"]));
            QMessageBox msgBox;
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setText(msg);
            msgBox.exec();
        }
        if (j["type"] == _REGISTER)
        {
            j = j["data"];
            int32_t id = j["user_id"];
            emit registerSuccess(id);
        }
        else if (j["type"] == _GET_FRIENDS)
        {
            j = j["data"];
            for (const auto &f : j)
            {
                int32_t id = f["friend_id"];
                QString name = QString::fromStdString(std::string(f["friend_name"]));
                emit newFriendRecieve(LiteChat_Dialog::Private, id, name);
            }
        }

        else if (j["type"] == _PRIVATE_MESSAGE){
             j = j["data"];
             int32_t fromId = j["from_id"], toId = j["to_id"];
             QString msg = QString::fromStdString(std::string(j["content"]));
             emit messageReceive(LiteChat_Dialog::Private, fromId, toId, msg);
        }

        else if (j["type"] == _GET_HISTORY_PRIVATE)
        {
            j = j["data"];
            for (const auto &f : j)
            {
                int32_t fromId = f["data"]["from_id"], toId = f["data"]["to_id"];
                QString msg = QString::fromStdString(std::string(f["data"]["content"]));
                emit messageReceive(LiteChat_Dialog::Private, fromId, toId, msg);
            }
        }

        else if (j["type"] == _GET_FRIEND_REQUEST)
        {
            j = j["data"];
            for (const auto &r : j)
            {
                int32_t id = r["from_id"];
                QString name = QString::fromStdString(std::string(r["name"]));
                emit friendRequestReceive(name, id);
            }
        }

        else if (j["type"] == _SEARCH_USER)
        {
            j = j["data"];
            for (const auto &u : j)
            {
                int32_t id = u["user_id"];
                QString name = QString::fromStdString(std::string(u["user_name"]));
                emit searchResultReceive(name, id);
            }
        }

        else if (j["type"] == _LOGIN)
        {
            j = j["data"];
            if (j["result"] == "login successfully")
            {
                userInfo.id = j["user_id"];
                userInfo.username = QString::fromStdString(std::string(j["user_name"]));
                token = QString::fromStdString(std::string(j["token"]));
                loginStatus = true;
                emit loginSuccess(userInfo.username, userInfo.id);
            }
        }
        else if (j["type"] == _GET_GROUPS)
        {
            j = j["data"];
            for (const auto &g : j)
            {
                int32_t id = g["group_id"];
                QString name = QString::fromStdString(std::string(g["group_name"]));
                emit newFriendRecieve(LiteChat_Dialog::Group, id, name);
            }
        }
        else if (j["type"] == _CREATE_GROUP)
        {
            j = j["data"];
            int32_t id = j["group_id"];
            QString name = QString::fromStdString(std::string(j["group_name"]));
            emit newFriendRecieve(LiteChat_Dialog::Group, id, name);
        }
        else if (j["type"] == _GROUP_MESSAGE)
        {
            j = j["data"];
            int32_t id = j["group_id"];
            QString msg = QString::fromStdString(std::string(j["content"]));
            emit messageReceive(LiteChat_Dialog::Group, id, userInfo.id, msg);
        }
        else if (j["type"] == _GET_HISTORY_GROUP)
        {
            j = j["data"];
            for (const auto &f : j)
            {
                int32_t groupId = f["data"]["group_id"], fromId = f["data"]["from_id"];
                QString msg = QString::fromStdString(std::string(f["data"]["content"]));
                if (fromId == userInfo.id) {
                    emit messageReceive(LiteChat_Dialog::Group, fromId, groupId, msg);
                }
                else {
                    emit messageReceive(LiteChat_Dialog::Private, groupId, userInfo.id, msg);
                }
            }
        }

    } catch (std::exception &e)
    {
        qDebug() << e.what() << '\n';
        qDebug() << "Invalid sequence received\n";
        qDebug() << "Which is : " << str << '\n';
    }
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

int LiteChat_Server::requestRegister(QString name, QString pwd, QString email)
{
    json j;
    j["type"] = _REGISTER;
    j["data"]["username"] = name.toUtf8();
    j["data"]["pwd"] = pwd.toUtf8();
    j["data"]["email"] = email.toUtf8();
    j["data"]["birthday"] = "2002-01-01";

    return sendtoServer(j);
}

int LiteChat_Server::sendMessage(LiteChat_Dialog::Dialog_Type dialogType, int32_t toId, QString msg)
{
    if (!loginStatus) return -1;
    json j;
    j["type"] = dialogType == LiteChat_Dialog::Private ? _PRIVATE_MESSAGE : _GROUP_MESSAGE;
    j["token"] = token.toUtf8();
    j["data"]["user_id"] = userInfo.id;
    if (dialogType == LiteChat_Dialog::Private) j["data"]["to_id"] = toId;
    else j["data"]["group_id"] = toId;
    j["data"]["content"] = msg.toUtf8();
    j["data"]["time"] = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz").toUtf8();
    emit messageReceive(dialogType, userInfo.id, toId, msg);
    return sendtoServer(j);
}

int LiteChat_Server::requestLogin(int32_t id, QString pwd)
{
    json j;
    j["type"] = _LOGIN;
    j["data"]["user_id"] = id;
    j["data"]["pwd"] = pwd.toUtf8();
    return sendtoServer(j);
}

int LiteChat_Server::requestMessages(int32_t toId)
{
    if (!loginStatus) return -1;
    json j;
    j["type"] = _GET_HISTORY_PRIVATE;
    j["token"] = token.toUtf8();
    j["data"]["user_id"] = userInfo.id;
    j["data"]["from_id"] = toId;
    return sendtoServer(j);
}

int LiteChat_Server::requestFriends()
{
    if (!loginStatus) return -1;
    json j;
    j["type"] = _GET_FRIENDS;
    j["token"] = token.toUtf8();
    j["data"]["user_id"] = userInfo.id;
    return sendtoServer(j);
}

int LiteChat_Server::searchUser(QString str)
{
    if (!loginStatus) return -1;
    json j;
    j["type"] = _SEARCH_USER;
    j["token"] = token.toUtf8();
    j["data"]["user_id"] = userInfo.id;
    j["data"]["keyword"] = str.toUtf8();
    return sendtoServer(j);
}

int LiteChat_Server::makeFriendRequest(int32_t id)
{
    if (!loginStatus) return -1;
    json j;
    j["type"] = _ADD_FRIEND;
    j["token"] = token.toUtf8();
    j["data"]["user_id"] = userInfo.id;
    j["data"]["to_id"] = id;
    j["data"]["message"] = "I dont need a message now!";
    return sendtoServer(j);
}

int LiteChat_Server::getFriendRequest(){
    if (!loginStatus) return -1;
    json j;
    j["type"] = _GET_FRIEND_REQUEST;
    j["token"] = token.toUtf8();
    j["data"]["user_id"] = userInfo.id;
    return sendtoServer(j);
}

int LiteChat_Server::acceptFriend(int32_t id, bool accept)
{
    if (!loginStatus) return -1;
    json j;
    j["type"] = _ACCEPT_FRIEND;
    j["token"] = token.toUtf8();
    j["data"]["from_id"] = id;
    j["data"]["user_id"] = userInfo.id;
    j["data"]["accept"] = accept;
    return sendtoServer(j);
}

int LiteChat_Server::deleteFriend(int32_t id)
{
    if (!loginStatus) return -1;
    json j;
    j["type"] = _DELETE_FRIEND;
    j["token"] = token.toUtf8();
    j["data"]["friend_id"] = id;
    j["data"]["user_id"] = userInfo.id;
    emit friendDeleted(LiteChat_Dialog::Private, id);
    return sendtoServer(j);
}

int LiteChat_Server::createGroup(QString groupName)
{
    if (!loginStatus) return -1;
    json j;
    j["type"] = _CREATE_GROUP;
    j["token"] = token.toUtf8();
    j["data"]["group_name"] = groupName.toUtf8();
    j["data"]["user_id"] = userInfo.id;
    j["data"]["group_description"] = "This is a group created by " + userInfo.username.toUtf8();
    return sendtoServer(j);
}

int LiteChat_Server::requestGroups()
{
    if (!loginStatus) return -1;
    json j;
    j["type"] = _GET_GROUPS;
    j["token"] = token.toUtf8();
    j["data"]["user_id"] = userInfo.id;
    return sendtoServer(j);
}

int LiteChat_Server::leaveGroup(int32_t id)
{
    if (!loginStatus) return -1;
    json j;
    j["type"] = _DELETE_MEMBER;
    j["token"] = token.toUtf8();
    j["data"]["member_id"] = userInfo.id;
    j["data"]["user_id"] = userInfo.id;
    j["data"]["group_id"] = id;
    emit friendDeleted(LiteChat_Dialog::Group, id);
    return sendtoServer(j);
}

int LiteChat_Server::inviteFriend(int32_t friendId, int32_t groupId)
{
    if (!loginStatus) return -1;
    json j;
    j["type"] = _INVITE_MEMBER;
    j["token"] = token.toUtf8();
    j["data"]["to_id"] = friendId;
    j["data"]["user_id"] = userInfo.id;
    j["data"]["group_id"] = groupId;
    return sendtoServer(j);
}

LiteChat_Login* LiteChat_Server::createLoginPage()
{
    LiteChat_Login *loginPage = new LiteChat_Login(this);
    connect(this, &LiteChat_Server::loginSuccess, loginPage, &LiteChat_Login::loginSuccess);
    return loginPage;
}

LiteChat_Dialog* LiteChat_Server::createDialog(QString dialogName, LiteChat_Dialog::Dialog_Type dialogType, int32_t toId)
{
    LiteChat_Dialog *dialogPage = new LiteChat_Dialog(this, dialogName, dialogType, toId);
    return dialogPage;
}

LiteChat_Interface* LiteChat_Server::createInterface(QString loginName, int32_t loginId){
    LiteChat_Interface *interface = new LiteChat_Interface(this, loginName, loginId);
    connect(this, &LiteChat_Server::messageReceive, interface, &LiteChat_Interface::messageReceive);
    connect(this, &LiteChat_Server::newFriendRecieve, interface, &LiteChat_Interface::addSingleDialogListItem);
    connect(this, &LiteChat_Server::friendDeleted, interface, &LiteChat_Interface::deleteSingleDialogListItem);
    return interface;
}

LiteChat_Register* LiteChat_Server::createRegister(){
    LiteChat_Register *Register = new LiteChat_Register(this);
    connect(this, &LiteChat_Server::registerSuccess, Register, &LiteChat_Register::registerSuccess);
    return Register;
}

LiteChat_FindUser* LiteChat_Server::createFindUser(){
    LiteChat_FindUser* findPage = new LiteChat_FindUser(this);
    connect(this, &LiteChat_Server::searchResultReceive, findPage, &LiteChat_FindUser::addSearchResult);
    connect(this, &LiteChat_Server::friendRequestReceive, findPage, &LiteChat_FindUser::addfriendRequest);
    getFriendRequest();
    return findPage;
}

LiteChat_InviteFriend* LiteChat_Server::createInvitePage(int32_t groupId)
{
    LiteChat_InviteFriend *invitePage = new LiteChat_InviteFriend(this, groupId);
    connect(this, &LiteChat_Server::newFriendRecieve, invitePage, &LiteChat_InviteFriend::addSingleFriend);
    return invitePage;
}
