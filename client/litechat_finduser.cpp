#include <QMessageBox>

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
    connect(ui->listWidget, &QListWidget::itemClicked, this, &LiteChat_FindUser::addFriendConfirm);
}

LiteChat_FindUser::~LiteChat_FindUser()
{
    delete ui;
}

void LiteChat_FindUser::on_pushButton_clicked()
{
    ui->listWidget->clear();
    liteChatServer->searchUser(ui->lineEdit->text());
}

void LiteChat_FindUser::addSearchResult(QString name, int32_t id)
{
    LiteChat_SearchListItem *newUser = new LiteChat_SearchListItem(id, name, ui->listWidget);
    QListWidgetItem *newItem = new QListWidgetItem(ui->listWidget);
    newItem->setSizeHint(QSize(ui->listWidget->size().width() - 10, 60));
    ui->listWidget->setItemWidget(newItem, newUser);
}

void LiteChat_FindUser::addFriendConfirm(QString name, int32_t id)
{
    QMessageBox message(QMessageBox::Question, "确认添加好友", "你想要添加 " + name + "(ID:" + id + ") 为好友吗？", QMessageBox::Yes | QMessageBox::No, NULL);
    if(message.exec() == QMessageBox::Yes)
    {
            liteChatServer->makeFriendRequest(id);
    }
    else return;
}
