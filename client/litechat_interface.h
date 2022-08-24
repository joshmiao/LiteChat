#ifndef LITECHAT_INTERFACE_H
#define LITECHAT_INTERFACE_H

#include <QMainWindow>
#include <QLabel>
#include <QDateTime>
#include "litechat_server.h"
#include "litechat_dialog.h"


namespace Ui {
class LiteChat_Interface;
}

class DialogInfo{
public:
    explicit DialogInfo(LiteChat_Dialog::Dialog_Type dialogType, int32_t toId, QString dialogName, QDateTime lastMessageTime, QString lastMessage = "");
    ~DialogInfo() = default;
    LiteChat_Dialog::Dialog_Type dialogType;
    int32_t toId;
    QString dialogName;
    QDateTime lastMessageTime;
    QString lastMessage;
    bool operator < (const DialogInfo &rhs) const {return lastMessageTime > rhs.lastMessageTime;}
};

class LiteChat_DialogListItem : public QWidget
{
    Q_OBJECT
public:
    explicit LiteChat_DialogListItem(LiteChat_Dialog::Dialog_Type dialogType, int32_t toId, QString dialogName, QString lastMessage = "", QWidget *parent = nullptr);
    ~LiteChat_DialogListItem() = default;
    LiteChat_Dialog::Dialog_Type dialogType;
    int32_t toId;
    QString dialogName, lastMessage;

private:
    QLabel *dialogNameLabel, *dialogContentLabel;
};


class LiteChat_Interface : public QMainWindow
{
    Q_OBJECT

public:
    explicit LiteChat_Interface(LiteChat_Server *liteChatServer, QString loginName, int32_t loginId, QWidget *parent = nullptr);
    ~LiteChat_Interface();


private:
    Ui::LiteChat_Interface *ui;
    LiteChat_Server *liteChatServer;

    UserInfo userinfo;

    LiteChat_Dialog *currentDialog;

    std::map<std::pair<LiteChat_Dialog::Dialog_Type, int32_t>, LiteChat_Dialog*> openedDialog;
    std::map<std::pair<LiteChat_Dialog::Dialog_Type, int32_t>, uint32_t> dialogListIndex;
    std::vector<DialogInfo> dialogInfoList;
    std::vector<int> mappingIndex;

    void flushDialogList();

public slots:
    void changeCurrentDialog(int currentRow);
    void messageReceive(LiteChat_Dialog::Dialog_Type dialogType, int32_t from_Id, int32_t toId, QString msg);
    void addSingleDialogListItem(LiteChat_Dialog::Dialog_Type dialogType, int32_t toId, QString dialogName);

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_lineEdit_textChanged(const QString &arg1);
};



#endif // LITECHAT_INTERFACE_H
