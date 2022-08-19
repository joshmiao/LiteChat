#include "litechat_diolog.h"
#include "ui_litechat_diolog.h"

#include <QDateTime>
#include <QDebug>

LiteChat_Diolog::LiteChat_Diolog(LiteChat *liteChatMain, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LiteChat_Diolog),
    liteChatMain(liteChatMain)
{
    ui->setupUi(this);
    resize(600, 800);
}

LiteChat_Diolog::~LiteChat_Diolog()
{
    delete ui;
}

void LiteChat_Diolog::on_pushButton_clicked()
{
    QString msg = ui->textEdit->toPlainText();
    liteChatMain->sendtoServer(msg);
    ui->textEdit->setText("");
    QString time = QString::number(QDateTime::currentDateTime().toTime_t()); //时间戳

    bool isSending = true; // 发送中

    qDebug()<<"addMessage" << msg << time << ui->listWidget->count();
    if(ui->listWidget->count()%2) {
        if(isSending) {
            dealMessageTime(time);

            LiteChat_Message* messageW = new LiteChat_Message(ui->listWidget->parentWidget());
            QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
            dealMessage(messageW, item, msg, time, LiteChat_Message::User_Me);
        } else {
            bool isOver = true;
            for(int i = ui->listWidget->count() - 1; i > 0; i--) {
                LiteChat_Message* messageW = (LiteChat_Message*)ui->listWidget->itemWidget(ui->listWidget->item(i));
                if(messageW->text() == msg) {
                    isOver = false;
                    messageW->setTextSuccess();
                }
            }
            if(isOver) {
                dealMessageTime(time);

                LiteChat_Message* messageW = new LiteChat_Message(ui->listWidget->parentWidget());
                QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
                dealMessage(messageW, item, msg, time, LiteChat_Message::User_Me);
                messageW->setTextSuccess();
            }
        }
    } else {
        if(msg != "") {
            dealMessageTime(time);

            LiteChat_Message* messageW = new LiteChat_Message(ui->listWidget->parentWidget());
            QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
            dealMessage(messageW, item, msg, time, LiteChat_Message::User_She);
        }
    }
    ui->listWidget->setCurrentRow(ui->listWidget->count()-1);
}

void LiteChat_Diolog::dealMessage(LiteChat_Message *messageW, QListWidgetItem *item, QString text, QString time,  LiteChat_Message::User_Type type)
{
    messageW->setFixedWidth(this->width());
    QSize size = messageW->fontRect(text);
    item->setSizeHint(size);
    messageW->setText(text, time, size, type);
    ui->listWidget->setItemWidget(item, messageW);
}

void LiteChat_Diolog::dealMessageTime(QString curMsgTime)
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

        QSize size = QSize(this->width(), 40);
        messageTime->resize(size);
        itemTime->setSizeHint(size);
        messageTime->setText(curMsgTime, curMsgTime, size, LiteChat_Message::User_Time);
        ui->listWidget->setItemWidget(itemTime, messageTime);
    }
}

void LiteChat_Diolog::resizeEvent(QResizeEvent *event){
    Q_UNUSED(event);
}
/*
void LiteChat_Diolog::resizeEvent(QResizeEvent *event)
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
