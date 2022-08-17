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
    client->connectToHost(ip, port.toShort());
    connect(client, SIGNAL(), this, SLOT(handConnected()));
}

void LiteChat::handConnected()
{
    ui->pushButton_2->setEnabled(true);
}

