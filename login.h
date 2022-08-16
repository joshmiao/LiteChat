#ifndef LOGIN_H
#define LOGIN_H

#include <QMainWindow>
#include <QDebug>
#include <QTcpSocket>
#include <QMessageBox>
#include <register.h>
#include <mainwindow.h>


QT_BEGIN_NAMESPACE
namespace Ui { class Login; }
QT_END_NAMESPACE

class Login : public QMainWindow
{
    Q_OBJECT

public:
    Login(QWidget *parent = nullptr);
    ~Login();

private slots:
    void handConnected();
    void on_pushButton_clicked();
    void handReadyRead();

    void on_pushButton_2_clicked();

private:
    Ui::Login *ui;
    QTcpSocket *client;
    Register *reg;
    MainWindow *mw;
};
#endif // LOGIN_H
