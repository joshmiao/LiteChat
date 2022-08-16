#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::MainWindow(QTcpSocket *sock, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    client = sock;
    connect(client, SIGNAL(readyRead()), this, SLOT(handReadyRead()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    QString msg = ui->lineEdit->text();
    QString packData = "#3|andy|" + msg + "&";
    QString showData = "我对andy说:" + msg;
    ui->textEdit->append(showData);
    client->write(packData.toLocal8Bit());
}

void MainWindow::handReadyRead()
{
    QByteArray recvArray = client->readAll();
    if(recvArray.at(0) != '#' || recvArray.at(recvArray.length() - 1) != '&')
    {
        return;
    }
    QString recvStr = QString::fromLocal8Bit(recvArray);
    recvStr = recvStr.mid(1, recvStr.length() - 2);
    QStringList recvList = recvStr.split('|');
    if(recvList.length() < 3)
    {
        return;
    }
    if(recvList[0] == "3")
    {
        QString recvMsg = recvList[1] + "对我说:" + recvList[2];
        ui->textEdit->append(recvMsg);
    }
}

