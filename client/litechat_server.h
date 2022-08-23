#ifndef LITECHAT_SERVER_H
#define LITECHAT_SERVER_H

#include <QMainWindow>
#include <QTcpSocket>
#include "litechat_dialog.h"

#include <json.hpp>
using json = nlohmann::json;

QT_BEGIN_NAMESPACE
namespace Ui { class LiteChat_Server; }
QT_END_NAMESPACE

class LiteChat_Login;
class LiteChat_Dialog;
class LiteChat_Interface;
class LiteChat_ChatList;
class LiteChat_Register;
class LiteChat_FindUser;

class UserInfo
{
public:
    explicit UserInfo(int32_t id, QString username = "");
    ~UserInfo() = default;
    int32_t id;
    QString username;
};

class LiteChat_Server : public QMainWindow
{
    Q_OBJECT

public:
    LiteChat_Server(QWidget *parent = nullptr);
    ~LiteChat_Server();

    LiteChat_Login* createLoginPage();
    LiteChat_Dialog* createDialog(QString dialogName, LiteChat_Dialog::Dialog_Type dialogType, int32_t toId);
    LiteChat_Interface* createInterface(QString loginName, int32_t loginId);
    LiteChat_Register* createRegister();
    LiteChat_FindUser* createFindUser();

    int requestRegister(QString name, QString pwd, QString email);
    int requestLogin(int32_t id, QString pwd);
    int requestFriends();
    int requestMessages(int32_t toId);
    int sendMessage(LiteChat_Dialog::Dialog_Type dialogType, int32_t toId, QString msg);
    int getFriendRequest();
    int acceptFriend(int32_t id, bool accept);

    int searchUser(QString str);
    int makeFriendRequest(int32_t id);

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void handConnected();
    void handReadyRead();

private:
    int sendtoServer(json j);
    void settleJson(QString str);
    Ui::LiteChat_Server *ui;
    QTcpSocket *client;
    bool serverStatus, loginStatus;
    UserInfo userInfo;
    QString token;

signals:
    void registerSuccess(int32_t id);
    void messageReceive(LiteChat_Dialog::Dialog_Type recieveType, int32_t fromId, int32_t toId, QString msg);
    void newFriendRecieve(LiteChat_Dialog::Dialog_Type recieveType, int32_t id, QString name);
    void loginSuccess(QString loginName, int32_t loginId);
    void searchResultReceive(QString name, int32_t id);
    void friendRequestReceive(QString name, int32_t id);
};
#endif // LITECHAT_SERVER_H
