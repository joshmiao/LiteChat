#include <QMessageBox>
#include "litechat_interface.h"
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

LiteChat_InviteFriend::LiteChat_InviteFriend(LiteChat_Server *liteChatServer, int32_t groupId, std::vector<DialogInfo> dialogInfo, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LiteChat_InviteFriend),
    liteChatServer(liteChatServer),
    groupId(groupId),
    dialogInfo(dialogInfo)
{
    ui->setupUi(this);
    qDebug() << "!!!!" << dialogInfo.size() << '\n';
    for (auto iter = dialogInfo.begin(); iter != dialogInfo.end(); ++iter){
        qDebug() << "now type" << iter->dialogType << '\n';
        qDebug() << "now Name" << iter->dialogName << '\n';
        if (iter->dialogType == LiteChat_Dialog::Group) dialogInfo.erase(iter);
        else {
            LiteChat_FriendListItem *newUser = new LiteChat_FriendListItem(iter->toId, iter->dialogName, ui->listWidget);
            QListWidgetItem *newItem = new QListWidgetItem(ui->listWidget);
            newItem->setSizeHint(QSize(ui->listWidget->size().width() - 10, 60));
            ui->listWidget->setItemWidget(newItem, newUser);
        }
    }

    connect(ui->listWidget, &QListWidget::currentRowChanged, this, &LiteChat_InviteFriend::inviteUser);
}

LiteChat_InviteFriend::~LiteChat_InviteFriend()
{
    delete ui;
}

void LiteChat_InviteFriend::inviteUser(int currentRow)
{
    qDebug() << "&&&&" << dialogInfo.size() << '\n';
    DialogInfo currentUser = dialogInfo[currentRow];
    QString name = currentUser.dialogName;
    int32_t id = currentUser.toId;
    QMessageBox message(QMessageBox::Question, "邀请群成员", "你想要邀请 " + name + "(ID:" + QString::fromStdString(std::to_string(id)) + ") 加入群吗？", QMessageBox::Yes | QMessageBox::No, NULL);
    if(message.exec() == QMessageBox::Yes)
    {
            liteChatServer->inviteFriend(id, groupId);
    }
}

