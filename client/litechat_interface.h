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
    explicit LiteChat_DialogListItem(LiteChat_Dialog::Dialog_Type dialogType, int32_t toId, QString chatName, QWidget *parent = nullptr);
    ~LiteChat_DialogListItem() = default;
    LiteChat_Dialog::Dialog_Type dialogType;
    int32_t toId;
    QString chatName;

private:
    QLabel *chatNameLabel, *chatContentLabel;
};

class LiteChat_Interface : public QMainWindow
{
    Q_OBJECT

public:
    explicit LiteChat_Interface(LiteChat_Server *liteChatServer, QString loginName, int32_t loginId, QWidget *parent = nullptr);
    ~LiteChat_Interface();
    void addSingleChatListItem(LiteChat_Dialog::Dialog_Type dialogType, int32_t toId, QString chatName);

private:
    Ui::LiteChat_Interface *ui;
    LiteChat_Server *liteChatServer;

    UserInfo userinfo;

    LiteChat_Dialog *currentDialog;

    std::map<std::pair<LiteChat_Dialog::Dialog_Type, int32_t>, LiteChat_Dialog*> openedDialog;
    std::map<int32_t, LiteChat_DialogListItem*> dialogList;

public slots:
    void changeCurrentDialog(QListWidgetItem *currentItem);

};



#endif // LITECHAT_INTERFACE_H
