#ifndef LITECHAT_PRIVATECHAT_H
#define LITECHAT_PRIVATECHAT_H

#include <QMainWindow>
#include "litechat.h"

namespace Ui {
class LiteChat_PrivateChat;
}

class LiteChat_PrivateChat : public QMainWindow
{
    Q_OBJECT

public slots:
    void showMessage(QString msg);

public:
    explicit LiteChat_PrivateChat(LiteChat *liteChatMain, QWidget *parent = nullptr);
    ~LiteChat_PrivateChat();

private slots:
    void on_pushButton_clicked();

private:
    Ui::LiteChat_PrivateChat *ui;
    LiteChat *liteChatMain;
};

#endif // LITECHAT_PRIVATECHAT_H
