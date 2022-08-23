#include <QMessageBox>
#include <QListWidget>

#include "litechat_server.h"
#include "litechat_finduser.h"
#include "ui_litechat_finduser.h"

LiteChat_SearchListItem:: LiteChat_SearchListItem(int32_t id, QString name, QWidget *parent) :
    QWidget(parent),
    id(id),
    name(name),
    resultNameLabel(new QLabel(name, this)),
    resultIdLabel(new QLabel(QString::fromStdString("ID: " + std::to_string(id)), this))
{
    QFont font;
    font.setPointSize(9);
    resize(QSize(parent->size().width(), 60));
    resultNameLabel->setFont(font);
    resultNameLabel->setAttribute(Qt::WA_TranslucentBackground);
    resultNameLabel->setGeometry(60, 5, parent->size().width() - 40, 20);
    resultIdLabel->setFont(font);
    resultIdLabel->setAttribute(Qt::WA_TranslucentBackground);
    resultIdLabel->setGeometry(60, 25, parent->size().width() - 40, 20);
}

LiteChat_FindUser::LiteChat_FindUser(LiteChat_Server *liteChatServer, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LiteChat_FindUser),
    liteChatServer(liteChatServer)
{
    ui->setupUi(this);
    connect(ui->listWidget, &QListWidget::currentRowChanged, this, &LiteChat_FindUser::addFriendConfirm);
    connect(ui->listWidget_2, &QListWidget::currentRowChanged, this, &LiteChat_FindUser::acceptFriend);
}

LiteChat_FindUser::~LiteChat_FindUser()
{
    delete ui;
}

void LiteChat_FindUser::on_pushButton_clicked()
{
    ui->listWidget->clear();
    searchList.clear();
    liteChatServer->searchUser(ui->lineEdit->text());
}

void LiteChat_FindUser::addSearchResult(QString name, int32_t id)
{
    LiteChat_SearchListItem *newUser = new LiteChat_SearchListItem(id, name, ui->listWidget);
    QListWidgetItem *newItem = new QListWidgetItem(ui->listWidget);
    newItem->setSizeHint(QSize(ui->listWidget->size().width() - 10, 60));
    ui->listWidget->setItemWidget(newItem, newUser);
    searchList[ui->listWidget->count() - 1] = newUser;
}

void LiteChat_FindUser::addFriendConfirm(int currentRow)
{
    LiteChat_SearchListItem *currentUser = searchList[currentRow];
    QString name = currentUser->name;
    int32_t id = currentUser->id;
    QMessageBox message(QMessageBox::Question, "确认想添加好友", "你想要添加 " + name + "(ID:" + QString::fromStdString(std::to_string(id)) + ") 为好友吗？", QMessageBox::Yes | QMessageBox::No, NULL);
    if(message.exec() == QMessageBox::Yes)
    {
            liteChatServer->makeFriendRequest(id);
    }
    else return;
}

void LiteChat_FindUser::addfriendRequest(QString name, int32_t id)
{
    if (requestSet.count({id, name})) return;
    requestSet.insert({id, name});
    LiteChat_SearchListItem *newUser = new LiteChat_SearchListItem(id, name, ui->listWidget_2);
    QListWidgetItem *newItem = new QListWidgetItem(ui->listWidget_2);
    newItem->setSizeHint(QSize(ui->listWidget_2->size().width() - 10, 60));
    ui->listWidget_2->setItemWidget(newItem, newUser);
    requestList[ui->listWidget_2->count() - 1] = newUser;
}

void LiteChat_FindUser::acceptFriend(int currentRow){
    LiteChat_SearchListItem *currentUser = requestList[currentRow];
    QString name = currentUser->name;
    int32_t id = currentUser->id;
    qDebug() << "current accept" << id << '\n';
    if (!requestSet.count({id, name})) return;
    QMessageBox message(QMessageBox::Question, "确认通过好友申请", "你想要通过 " + name + "(ID:" + QString::fromStdString(std::to_string(id)) + ") 的好友申请吗？", QMessageBox::Yes | QMessageBox::No, NULL);
    if(message.exec() == QMessageBox::Yes)
    {
            liteChatServer->acceptFriend(id, 1);
            requestSet.erase({id, name});
    }
    else liteChatServer->acceptFriend(id, 0);

    disconnect(ui->listWidget_2, &QListWidget::currentRowChanged, this, &LiteChat_FindUser::acceptFriend);
    ui->listWidget_2->clear();
    requestList.clear();
    for (const auto &[id, name] : requestSet)
    {
        LiteChat_SearchListItem *newUser = new LiteChat_SearchListItem(id, name, ui->listWidget_2);
        QListWidgetItem *newItem = new QListWidgetItem(ui->listWidget_2);
        newItem->setSizeHint(QSize(ui->listWidget_2->size().width() - 10, 60));
        ui->listWidget_2->setItemWidget(newItem, newUser);
        requestList[ui->listWidget_2->count() - 1] = newUser;
    }
    connect(ui->listWidget_2, &QListWidget::currentRowChanged, this, &LiteChat_FindUser::acceptFriend);
}