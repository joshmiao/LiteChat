#ifndef LITECHAT_INTERFACE_H
#define LITECHAT_INTERFACE_H

#include <QMainWindow>
#include <QLabel>
#include "litechat_server.h"
#include "litechat_dialog.h"

namespace Ui {
class LiteChat_Interface;
}

class LiteChat_DialogListItem : public QWidget
{
    Q_OBJECT
public:
    explicit LiteChat_DialogListItem(LiteChat_Dialog::Dialog_Type dialogType, int32_t toId, QString dialogName, QWidget *parent = nullptr);
    ~LiteChat_DialogListItem() = default;
    LiteChat_Dialog::Dialog_Type dialogType;
    int32_t toId;
    QString dialogName;

private:
    QLabel *dialogNameLabel, *dialogContentLabel;
};

class LiteChat_Interface : public QMainWindow
{
    Q_OBJECT

public:
    explicit LiteChat_Interface(LiteChat_Server *liteChatServer, QString loginName, int32_t loginId, QWidget *parent = nullptr);
    ~LiteChat_Interface();
    void addSingleDialogListItem(LiteChat_Dialog::Dialog_Type dialogType, int32_t toId, QString dialogName);

private:
    Ui::LiteChat_Interface *ui;
    LiteChat_Server *liteChatServer;

    UserInfo userinfo;

    LiteChat_Dialog *currentDialog;

    std::map<std::pair<LiteChat_Dialog::Dialog_Type, int32_t>, LiteChat_Dialog*> openedDialog;
    std::map<int32_t, LiteChat_DialogListItem*> dialogList;

public slots:
    void changeCurrentDialog(int currentRow);
    void messageReceive(LiteChat_Dialog::Dialog_Type dialogType, int32_t toId, QString chatName, QString msg);
};



#endif // LITECHAT_INTERFACE_H
