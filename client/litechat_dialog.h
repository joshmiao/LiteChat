#ifndef LITECHAT_DIALOG_H
#define LITECHAT_DIALOG_H

#include <QWidget>
#include <QPainter>
#include <QListWidgetItem>
#include "litechat_message.h"

namespace Ui {
class LiteChat_Dialog;
}

class LiteChat_Server;
class DialogInfo;

class LiteChat_Dialog : public QWidget
{
    Q_OBJECT
private:
    Ui::LiteChat_Dialog *ui;
    LiteChat_Server *liteChatServer;

public:
    enum Dialog_Type{
        Default,
        Private,
        Group,
    };
    QString dialogName;
    Dialog_Type dialogType;
    int32_t userId, toId;
    explicit LiteChat_Dialog(LiteChat_Server *liteChatServer, QString dialogName, Dialog_Type dialogType, int toId, QWidget *parent = nullptr);
    ~LiteChat_Dialog();
    void dealMessage(LiteChat_Message *messageW, QListWidgetItem *item, QString text, QString time, LiteChat_Message::User_Type type);
    void dealMessageTime(QString curMsgTime);
    void paintEvent(QPaintEvent *event);

public slots:
    void receiveSingalMessage(QString msg, bool myMessage);
protected:
    void resizeEvent(QResizeEvent *event);
private slots:
    void on_pushButton_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_4_clicked();




};

#endif // LITECHAT_DIALOG_H
