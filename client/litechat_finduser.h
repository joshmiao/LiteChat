#ifndef LITECHAT_FINDUSER_H
#define LITECHAT_FINDUSER_H

#include <QListWidget>
#include <QWidget>
#include <QLabel>
#include <set>

namespace Ui {
class LiteChat_FindUser;
}

class LiteChat_Server;

class LiteChat_SearchListItem : public QWidget
{
    Q_OBJECT
public:
    explicit LiteChat_SearchListItem(int32_t id, QString name, QWidget *parent = nullptr);
    ~LiteChat_SearchListItem() = default;
    int32_t id;
    QString name;

private:
    QLabel *resultNameLabel, *resultIdLabel;
};


class LiteChat_FindUser : public QWidget
{
    Q_OBJECT
public slots:
    void addSearchResult(QString name, int32_t id);
    void addfriendRequest(QString name, int32_t id);
public:
    explicit LiteChat_FindUser(LiteChat_Server *liteChatServer, QWidget *parent = nullptr);
    ~LiteChat_FindUser();

private slots:
    void on_pushButton_clicked();
    void addFriendConfirm(int currentRow);
    void acceptFriend(int currentRow);

private:
    Ui::LiteChat_FindUser *ui;
    LiteChat_Server *liteChatServer;

    std::map<uint32_t, LiteChat_SearchListItem*> searchList;
    std::map<uint32_t, LiteChat_SearchListItem*> requestList;
    std::set<std::pair<int32_t, QString>> requestSet;
};

#endif // LITECHAT_FINDUSER_H
