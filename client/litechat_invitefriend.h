#ifndef LITECHAT_INVITEFRIEND_H
#define LITECHAT_INVITEFRIEND_H

#include <QWidget>
#include <QLabel>
#include <QPoint>

namespace Ui {
class LiteChat_InviteFriend;
}

class LiteChat_Server;
class DialogInfo;

class LiteChat_FriendListItem : public QWidget
{
    Q_OBJECT
public:
    explicit LiteChat_FriendListItem(int32_t id, QString name, QWidget *parent = nullptr);
    ~LiteChat_FriendListItem() = default;
    int32_t id;
    QString name;


private:
    QLabel *NameLabel, *IdLabel;
};


class LiteChat_InviteFriend : public QWidget
{
    Q_OBJECT

public:
    explicit LiteChat_InviteFriend(LiteChat_Server *liteChatServer, int32_t groupId, std::vector<DialogInfo>, QWidget *parent = nullptr);
    ~LiteChat_InviteFriend();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    Ui::LiteChat_InviteFriend *ui;
    LiteChat_Server *liteChatServer;
    int32_t groupId;
    std::vector<DialogInfo> dialogInfo;
    QPoint m_mousePoint;
    QPoint movePoint;
    bool mousePress;

private slots:
    void inviteUser(int currentRow);
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_pressed();
    void on_pushButton_released();
    void on_pushButton_2_pressed();
    void on_pushButton_2_released();
};

#endif // LITECHAT_INVITEFRIEND_H
