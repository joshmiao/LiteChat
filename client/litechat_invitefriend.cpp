#include <QMessageBox>
#include "litechat_invitefriend.h"
#include "litechat_server.h"
#include "ui_litechat_invitefriend.h"


LiteChat_FriendListItem:: LiteChat_FriendListItem(int32_t id, QString name, QWidget *parent) :
    QWidget(parent),
    id(id),
    name(name),
    NameLabel(new QLabel(name, this)),
    IdLabel(new QLabel(QString::fromStdString("ID: " + std::to_string(id)), this))
{
    QFont font;
    font.setPointSize(9);
    resize(QSize(parent->size().width(), 60));
    NameLabel->setFont(font);
    NameLabel->setAttribute(Qt::WA_TranslucentBackground);
    NameLabel->setGeometry(60, 5, parent->size().width() - 40, 20);
    IdLabel->setFont(font);
    IdLabel->setAttribute(Qt::WA_TranslucentBackground);
    IdLabel->setGeometry(60, 25, parent->size().width() - 40, 20);
}

LiteChat_InviteFriend::LiteChat_InviteFriend(LiteChat_Server *liteChatServer, int32_t groupId, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LiteChat_InviteFriend),
    liteChatServer(liteChatServer),
    groupId(groupId)
{
    ui->setupUi(this);
    connect(ui->listWidget, &QListWidget::currentRowChanged, this, &LiteChat_InviteFriend::inviteUser);
    liteChatServer->requestFriends();
}

void LiteChat_InviteFriend::addSingleFriend(LiteChat_Dialog::Dialog_Type type, int32_t id, QString name)
{
    if (type == LiteChat_Dialog::Group) return;
    LiteChat_FriendListItem *newUser = new LiteChat_FriendListItem(id, name, ui->listWidget);
    QListWidgetItem *newItem = new QListWidgetItem(ui->listWidget);
    newItem->setSizeHint(QSize(ui->listWidget->size().width() - 10, 60));
    ui->listWidget->setItemWidget(newItem, newUser);
    friendListIndex[ui->listWidget->count() - 1] = {id, name};
}

LiteChat_InviteFriend::~LiteChat_InviteFriend()
{
    delete ui;
}

void LiteChat_InviteFriend::inviteUser(int currentRow)
{
    auto currentUser = friendListIndex[currentRow];
    QString name = currentUser.second;
    int32_t id = currentUser.first;
    QMessageBox message(QMessageBox::Question, "邀请群成员", "你想要邀请 " + name + "(ID:" + QString::fromStdString(std::to_string(id)) + ") 加入群吗？", QMessageBox::Yes | QMessageBox::No, NULL);
    if(message.exec() == QMessageBox::Yes)
    {
            liteChatServer->inviteFriend(id, groupId);
    }
    disconnect(ui->listWidget, &QListWidget::currentRowChanged, this, &LiteChat_InviteFriend::inviteUser);
    disconnect(liteChatServer, &LiteChat_Server::newFriendRecieve, this, &LiteChat_InviteFriend::addSingleFriend);
    delete this;
}

