#ifndef LITECHAT_DIALOG_H
#define LITECHAT_DIALOG_H

#include <QWidget>
#include <QListWidgetItem>
#include "litechat_message.h"

namespace Ui {
class LiteChat_Dialog;
}

class LiteChatServer;

class LiteChat_Dialog : public QWidget
{
    Q_OBJECT

public:
    enum Dialog_Type{
        Default,
        Private,
        Group,
    };
    explicit LiteChat_Dialog(LiteChatServer *liteChatServer, QString chatName, Dialog_Type dialogType, int toId, QWidget *parent = nullptr);
    ~LiteChat_Dialog();
    void dealMessage(LiteChat_Message *messageW, QListWidgetItem *item, QString text, QString time, LiteChat_Message::User_Type type);
    void dealMessageTime(QString curMsgTime);
public slots:
    void receiveSingalMessage(QString msg);
protected:
    void resizeEvent(QResizeEvent *event);
private slots:
    void on_pushButton_clicked();


private:
    Ui::LiteChat_Dialog *ui;
    LiteChatServer *liteChatServer;
    QString chatName;
    Dialog_Type dialogType;
    int32_t userId, toId;

};

#endif // LITECHAT_DIALOG_H
