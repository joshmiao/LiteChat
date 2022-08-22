#ifndef LITECHAT_INTERFACE_H
#define LITECHAT_INTERFACE_H

#include <QMainWindow>
#include "litechat.h"
#include "litechat_dialog.h"

namespace Ui {
class LiteChat_Interface;
}

class LiteChat_chatList;

class LiteChat_Interface : public QMainWindow
{
    Q_OBJECT

public:
    explicit LiteChat_Interface(LiteChat *liteChatMain, QString loginName, int32_t loginId, QWidget *parent = nullptr);
    ~LiteChat_Interface();

private:
    Ui::LiteChat_Interface *ui;
    LiteChat *liteChatMain;

    QString loginName;
    int32_t loginId;

    LiteChat_Dialog *currentDialog;
    LiteChat_ChatList *chatList;

    std::map<std::pair<LiteChat_Dialog::Dialog_Type, int32_t>, LiteChat_Dialog*> dialogList;

};

#endif // LITECHAT_INTERFACE_H
