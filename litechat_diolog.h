#ifndef LITECHAT_DIOLOG_H
#define LITECHAT_DIOLOG_H

#include <QWidget>
#include <QListWidgetItem>
#include "litechat.h"
#include "litechat_message.h"

namespace Ui {
class LiteChat_Diolog;
}

class LiteChat_Diolog : public QWidget
{
    Q_OBJECT

public:
    explicit LiteChat_Diolog(LiteChat *liteChatMain, QString title, QWidget *parent = nullptr);
    ~LiteChat_Diolog();
    void dealMessage(LiteChat_Message *messageW, QListWidgetItem *item, QString text, QString time, LiteChat_Message::User_Type type);
    void dealMessageTime(QString curMsgTime);
public slots:
    void receiveSingalMessage(QString msg);
protected:
    void resizeEvent(QResizeEvent *event);
private slots:
    void on_pushButton_clicked();


private:
    Ui::LiteChat_Diolog *ui;
    LiteChat *liteChatMain;
    QString title;
};

#endif // LITECHAT_DIOLOG_H
