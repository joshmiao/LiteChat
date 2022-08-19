#ifndef LITECHAT_H
#define LITECHAT_H

#include <QMainWindow>
#include <QTcpSocket>


QT_BEGIN_NAMESPACE
namespace Ui { class LiteChat; }
QT_END_NAMESPACE

class LiteChat : public QMainWindow
{
    Q_OBJECT

public:
    LiteChat(QWidget *parent = nullptr);
    ~LiteChat();
    int sendtoServer(QString msg);
    void createPrivateChat();
    void createLoginPage();
    void createDiolog();

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
