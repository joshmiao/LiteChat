#ifndef LITECHAT_INTERFACE_H
#define LITECHAT_INTERFACE_H

#include <QMainWindow>
#include <QLabel>
#include <QDateTime>
#include <QPoint>
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
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);


private:
    Ui::LiteChat_Interface *ui;
    LiteChat_Server *liteChatServer;

    UserInfo userinfo;

    LiteChat_Dialog *currentDialog;

    std::map<std::pair<LiteChat_Dialog::Dialog_Type, int32_t>, LiteChat_Dialog*> openedDialog;
    std::map<std::pair<LiteChat_Dialog::Dialog_Type, int32_t>, uint32_t> dialogListIndex;
    std::vector<DialogInfo> dialogInfoList;
    std::vector<int> mappingIndex;
    QPoint m_mousePoint;
    QPoint movePoint;
    bool mousePress;

    void flushDialogList();

public slots:
    void changeCurrentDialog(int currentRow);
    void messageReceive(LiteChat_Dialog::Dialog_Type dialogType, int32_t from_Id, int32_t toId, QString msg);
    void addSingleDialogListItem(LiteChat_Dialog::Dialog_Type dialogType, int32_t toId, QString dialogName);
    void deleteSingleDialogListItem(LiteChat_Dialog::Dialog_Type dialogType, int32_t toId);

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_lineEdit_textChanged(const QString &arg1);
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_pushButton_5_clicked();
    void on_pushButton_4_pressed();
    void on_pushButton_4_released();
    void on_pushButton_5_pressed();
    void on_pushButton_5_released();
    void on_pushButton_2_pressed();
    void on_pushButton_2_released();
    void on_pushButton_3_pressed();
    void on_pushButton_3_released();
};



#endif // LITECHAT_INTERFACE_H
