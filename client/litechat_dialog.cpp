#include "litechat_server.h"
#include "litechat_dialog.h"
#include "litechat_invitefriend.h"
#include "litechat_interface.h"
#include "ui_litechat_dialog.h"

#include <QDateTime>
#include <QDebug>
#include <QMessageBox>

LiteChat_Dialog::LiteChat_Dialog(LiteChat_Server *liteChatServer, QString dialogName, LiteChat_Dialog::Dialog_Type dialogType, int32_t toId, std::vector<DialogInfo> &dialogInfo, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LiteChat_Dialog),
    liteChatServer(liteChatServer),
    dialogName(dialogName),
    dialogType(dialogType),
    toId(toId),
    dialogInfo(dialogInfo)
{
    ui->setupUi(this);
    ui->label->setText(dialogName);
    if (dialogType == Private) delete(ui->pushButton_4);
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
    liteChatServer->sendMessage(dialogType, toId, msg);
}

void LiteChat_Dialog::receiveSingalMessage(QString msg, bool myMessage){
    qDebug() << "receive msg : " << msg << '\n';
    QString time = QString::number(QDateTime::currentDateTime().toTime_t()); //时间戳
    if(msg != "") {
        dealMessageTime(time);
        LiteChat_Message* messageW = new LiteChat_Message(ui->listWidget->parentWidget());
        QListWidgetItem* item = new QListWidgetItem(ui->listWidget);
        if (myMessage) {
            dealMessage(messageW, item, msg, time, LiteChat_Message::User_Me);
            messageW->setTextSuccess();
        }
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

void LiteChat_Dialog::on_pushButton_5_clicked()
{
    QString typemsg = dialogType == Private ? "删除好友" : "退出群聊";
    QMessageBox message(QMessageBox::Question, typemsg, "你想要 " + typemsg + dialogName + "(ID:" + QString::fromStdString(std::to_string(toId)) + ") 吗？", QMessageBox::Yes | QMessageBox::No, NULL);
    auto result = message.exec();
    if(result == QMessageBox::Yes)
    {
            if (dialogType == Private) liteChatServer->deleteFriend(toId);
            else if (dialogType == Group) liteChatServer->leaveGroup(toId);
            delete this;
    }
}


void LiteChat_Dialog::on_pushButton_4_clicked()
{
    LiteChat_InviteFriend *invitePage = new LiteChat_InviteFriend(liteChatServer, toId, dialogInfo);
    invitePage->show();
}

void LiteChat_Dialog::paintEvent(QPaintEvent *event)
{
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
