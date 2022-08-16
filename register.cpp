#include "register.h"
#include "ui_register.h"

Register::Register(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Register)
{
    ui->setupUi(this);
}

Register::Register(QTcpSocket *sock, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Register)
{
    ui->setupUi(this);
    client = sock;
}


Register::~Register()
{
    delete ui;
}

void Register::on_pushButton_clicked()
{
    QString uName = ui->lineEdit->text();
    if(ui->lineEdit_2->text() != ui->lineEdit_3->text())
    {
        QMessageBox::warning(this, "警告", "两次输入密码不一致");
        ui->lineEdit_3->clear();
        return;
    }
    QString pWord = ui->lineEdit_2->text();
    QString pNo = ui->lineEdit_4->text();
    QString bDay = ui->dateEdit->text();
    QString packData = "#2|" + uName + "|" + pWord + "|" + pNo + "|" + bDay + "&";
    client->write(packData.toLocal8Bit());
}








