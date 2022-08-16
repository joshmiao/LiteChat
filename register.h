#ifndef REGISTER_H
#define REGISTER_H

#include <QMainWindow>
#include <QDebug>
#include <QTcpSocket>
#include <QMessageBox>

namespace Ui {
class Register;
}

class Register : public QMainWindow
{
    Q_OBJECT

public:
    explicit Register(QWidget *parent = nullptr);
    explicit Register(QTcpSocket *sock, QWidget *parent = nullptr);
    ~Register();

private slots:
    void on_pushButton_clicked();

private:
    Ui::Register *ui;
    QTcpSocket *client;
};

#endif // REGISTER_H
