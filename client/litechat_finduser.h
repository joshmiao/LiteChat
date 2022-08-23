#ifndef LITECHAT_FINDUSER_H
#define LITECHAT_FINDUSER_H

#include <QWidget>
#include <QLabel>

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

public:
    explicit LiteChat_FindUser(LiteChat_Server *liteChatServer, QWidget *parent = nullptr);
    ~LiteChat_FindUser();

private slots:
    void on_pushButton_clicked();
    void addFriendConfirm(QString name, int32_t id);

private:
    Ui::LiteChat_FindUser *ui;
    LiteChat_Server *liteChatServer;
};

#endif // LITECHAT_FINDUSER_H
