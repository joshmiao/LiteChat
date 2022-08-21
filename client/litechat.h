#ifndef LITECHAT_H
#define LITECHAT_H

#include <QMainWindow>
#include <QTcpSocket>


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
    int sendtoServer(QString msg);
    LiteChat_Login* createLoginPage();
    LiteChat_Dialog* createDialog();
    LiteChat_Interface* createInterface();
    LiteChat_ChatList* createChatList();
    LiteChat_Register* createRegister();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void handConnected();
    void handReadyRead();

private:
    Ui::LiteChat *ui;
    QTcpSocket *client;
    bool serverReady;

signals:
    void messageReceive(QString msg);
};
#endif // LITECHAT_H
