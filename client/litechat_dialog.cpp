#include "litechat_server.h"
#include "litechat_dialog.h"
#include "ui_litechat_dialog.h"

#include <QDateTime>
#include <QDebug>

LiteChat_Dialog::LiteChat_Dialog(LiteChat_Server *liteChatServer, QString dialogName, Dialog_Type dialogType, int32_t toId, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LiteChat_Dialog),
    liteChatServer(liteChatServer),
    dialogName(dialogName),
    dialogType(dialogType),
    toId(toId)
{
    ui->setupUi(this);
    ui->label->setText(dialogName);
}

LiteChat_Dialog::~LiteChat_Dialog()
{
    delete ui;
}

void LiteChat_Dialog::on_pushButton_clicked()
{
    QString msg = ui->textEdit->toPlainText();

    ui->textEdit->setText("");
    QString time = QString::number(QDateTime::currentDateTime().toTime_t()); //时间戳

    qDebug()<<"addMessage" << msg << time << ui->listWidget->count();

    dealMessageTime(time);
    LiteChat_Message* messageW = new LiteChat_Message(ui->listWidget->parentWidget());
    QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
    dealMessage(messageW, item, msg, time, LiteChat_Message::User_Me);

    if(liteChatServer->sendMessage(dialogType, toId, msg) == 0) messageW->setTextSuccess();

    ui->listWidget->setCurrentRow(ui->listWidget->count()-1);
}

void LiteChat_Dialog::receiveSingalMessage(QString msg, bool myMessage){
    qDebug() << "receive msg : " << msg << '\n';
    QString time = QString::number(QDateTime::currentDateTime().toTime_t()); //时间戳
    if(msg != "") {
        dealMessageTime(time);
        LiteChat_Message* messageW = new LiteChat_Message(ui->listWidget->parentWidget());
        QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
        if (myMessage) dealMessage(messageW, item, msg, time, LiteChat_Message::User_Me);
        else dealMessage(messageW, item, msg, time, LiteChat_Message::User_She);
    }
    ui->listWidget->setCurrentRow(ui->listWidget->count()-1);
}

void LiteChat_Dialog::dealMessage(LiteChat_Message *messageW, QListWidgetItem *item, QString text, QString time,  LiteChat_Message::User_Type type)
{
    messageW->setFixedWidth(this->width() - 10);
    QSize size = messageW->fontRect(text);
    item->setSizeHint(size);
    messageW->setText(text, time, size, type);
    ui->listWidget->setItemWidget(item, messageW);
}

void LiteChat_Dialog::dealMessageTime(QString curMsgTime)
{
    bool isShowTime = false;
    if(ui->listWidget->count() > 0) {
        QListWidgetItem* lastItem = ui->listWidget->item(ui->listWidget->count() - 1);
        LiteChat_Message* messageW = (LiteChat_Message*)ui->listWidget->itemWidget(lastItem);
        int lastTime = messageW->time().toInt();
        int curTime = curMsgTime.toInt();
        qDebug() << "curTime lastTime:" << curTime - lastTime;
        isShowTime = ((curTime - lastTime) > 60); // 两个消息相差一分钟
//        isShowTime = true;
    } else {
        isShowTime = true;
    }
    if(isShowTime) {
        LiteChat_Message* messageTime = new LiteChat_Message(ui->listWidget->parentWidget());
        QListWidgetItem* itemTime = new QListWidgetItem(ui->listWidget);

        QSize size = QSize(this->width() - 10, 20);
        messageTime->resize(size);
        itemTime->setSizeHint(size);
        messageTime->setText(curMsgTime, curMsgTime, size, LiteChat_Message::User_Time);
        ui->listWidget->setItemWidget(itemTime, messageTime);
    }
}

void LiteChat_Dialog::resizeEvent(QResizeEvent *event){
    Q_UNUSED(event);
}

/*
void LiteChat_Dialog::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);


    ui->textEdit->resize(this->width() - 20, this->height() - 20);
    ui->textEdit->move(10, 10);

    ui->pushButton->move(ui->textEdit->width()+ui->textEdit->x() - ui->pushButton->width() - 10,
                         ui->textEdit->height()+ui->textEdit->y() - ui->pushButton->height() - 10);


    for(int i = 0; i < ui->listWidget->count(); i++) {
        LiteChat_Message* messageW = (LiteChat_Message*)ui->listWidget->itemWidget(ui->listWidget->item(i));
        QListWidgetItem* item = ui->listWidget->item(i);

        dealMessage(messageW, item, messageW->text(), messageW->time(), messageW->userType());
    }
}
*/
