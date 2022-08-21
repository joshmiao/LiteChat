#ifndef LITECHAT_INTERFACE_H
#define LITECHAT_INTERFACE_H

#include <QMainWindow>
#include "litechat.h"
#include "litechat_dialog.h"
#include "litechat_chatlist.h"

namespace Ui {
class LiteChat_Interface;
}

class LiteChat_Interface : public QMainWindow
{
    Q_OBJECT

public:
    explicit LiteChat_Interface(LiteChat *liteChatMain, QWidget *parent = nullptr);
    ~LiteChat_Interface();

private:
    Ui::LiteChat_Interface *ui;
    LiteChat *liteChatMain;
    LiteChat_Dialog *currentDialog;
    LiteChat_ChatList *chatList;
};

#endif // LITECHAT_INTERFACE_H
