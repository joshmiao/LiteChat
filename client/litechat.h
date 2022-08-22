#ifndef LITECHAT_H
#define LITECHAT_H

#include <QMainWindow>
#include <QTcpSocket>
#include "litechat_dialog.h"

#include <json.hpp>
using json = nlohmann::json;

QT_BEGIN_NAMESPACE
namespace Ui { class LiteChat; }
QT_END_NAMESPACE

class LiteChat_Login;
class LiteChat_Dialog;
class LiteChat_Interface;
class LiteChat_ChatList;
class LiteChat_Register;

class LiteChat : public QMainWindow
{
    Q_OBJECT

public:
    LiteChat(QWidget *parent = nullptr);
    ~LiteChat();
    LiteChat_Login* createLoginPage();
    LiteChat_Dialog* createDialog(QString chatName, LiteChat_Dialog::Dialog_Type dialogType, int id);
    LiteChat_Interface* createInterface(QString loginName, int32_t loginId);
    LiteChat_ChatList* createChatList();
    LiteChat_Register* createRegister();
    int sendMessage(LiteChat_Dialog::Dialog_Type dialogType, int32_t id, QString msg);
    int requestLogin(int32_t id, QString pwd);

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void handConnected();
    void handReadyRead();

private:
    int sendtoServer(json j);
    Ui::LiteChat *ui;
    QTcpSocket *client;
    bool serverReady;
    QString token;

signals:
    void messageReceive(QString msg);
    void loginSuccess(QString loginName, int32_t loginId);
};
#endif // LITECHAT_H
