#ifndef LITECHAT_DIALOG_H
#define LITECHAT_DIALOG_H

#include <QWidget>
#include <QListWidgetItem>
#include "litechat_message.h"

namespace Ui {
class LiteChat_Dialog;
}

class LiteChat_Server;

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
    QString chatName;
    Dialog_Type dialogType;
    int32_t userId, toId;
    explicit LiteChat_Dialog(LiteChat_Server *liteChatServer, QString chatName, Dialog_Type dialogType, int toId, QWidget *parent = nullptr);
    ~LiteChat_Dialog();
    void dealMessage(LiteChat_Message *messageW, QListWidgetItem *item, QString text, QString time, LiteChat_Message::User_Type type);
    void dealMessageTime(QString curMsgTime);
public slots:
    void receiveSingalMessage(QString msg);
protected:
    void resizeEvent(QResizeEvent *event);
private slots:
    void on_pushButton_clicked();


};

#endif // LITECHAT_DIALOG_H
