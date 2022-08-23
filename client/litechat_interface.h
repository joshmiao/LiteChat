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
    std::map<uint32_t, LiteChat_DialogListItem*> dialogList;
    std::map<std::pair<LiteChat_Dialog::Dialog_Type, int32_t>, uint32_t> dialogListIndex;

public slots:
    void changeCurrentDialog(int currentRow);
    void messageReceive(LiteChat_Dialog::Dialog_Type dialogType, int32_t from_Id, int32_t toId, QString msg);

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
};



#endif // LITECHAT_INTERFACE_H
