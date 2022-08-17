#include "litechat.h"
#include "ui_litechat.h"

LiteChat::LiteChat(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::LiteChat)
{
    ui->setupUi(this);
    client = new QTcpSocket();
}

LiteChat::~LiteChat()
{
    delete ui;
}


void LiteChat::on_pushButton_clicked()
{
    QString ip = ui->lineEdit->text();
    QString port = ui->lineEdit_2->text();
    if (ip == "" || port == ""){
        ui->textEdit->append("Input correct IP and Port!");
        return;
    }
    client->connectToHost(ip, port.toShort());
    ui->textEdit->append("tring to connect...");
    connect(client, SIGNAL(connected()), this, SLOT(handConnected()));
}

void LiteChat::handConnected()
{
    ui->pushButton_2->setEnabled(true);
    ui->textEdit->append("successfully connected!");
    connect(client, SIGNAL(readyRead()), this, SLOT(handReadyRead()));
}

void LiteChat::on_pushButton_2_clicked()
{
    QString txt = ui->lineEdit_3->text();
    client->write(txt.toUtf8());
}

void LiteChat::handReadyRead(){
    QByteArray recvArray = client->readAll();
    ui->textEdit->append(QString::fromUtf8(recvArray));
}
