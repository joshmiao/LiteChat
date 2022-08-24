#include <QMessageBox>
#include <QMouseEvent>
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
    font.setBold(true);
    NameLabel->setFont(font);
    NameLabel->setAttribute(Qt::WA_TranslucentBackground);
    NameLabel->setGeometry(60, 5, parent->size().width() - 40, 20);
    font.setBold(false);
    IdLabel->setFont(font);
    IdLabel->setAttribute(Qt::WA_TranslucentBackground);
    IdLabel->setGeometry(60, 25, parent->size().width() - 40, 20);
}

void LiteChat_FriendListItem::paintEvent(QPaintEvent *)
{
    QPixmap pixmap;
    QString url;
    url = QString::fromStdString(std::string(":/img/head") + std::to_string(id % 31) + std::string(".png"));
    pixmap.load(url);

    QPainter painter(this);
    painter.drawPixmap(10, 10, 40, 40, pixmap);
}

LiteChat_InviteFriend::LiteChat_InviteFriend(LiteChat_Server *liteChatServer, int32_t groupId, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LiteChat_InviteFriend),
    liteChatServer(liteChatServer),
    groupId(groupId)
{
    ui->setupUi(this);
    connect(ui->listWidget, &QListWidget::currentRowChanged, this, &LiteChat_InviteFriend::inviteUser);
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setFixedSize(this->width(),this->height());
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
    this->close();
}

void LiteChat_InviteFriend::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
        {
            mousePress = true;
        }
        movePoint = event->globalPos() - pos();
}

void LiteChat_InviteFriend::mouseMoveEvent(QMouseEvent *event)
{
    if(mousePress)
    {
        QPoint movePos = event->globalPos();
        move(movePos - movePoint);
    }
}

void LiteChat_InviteFriend::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    mousePress = false;
}

void LiteChat_InviteFriend::on_pushButton_clicked()
{
    this->showMinimized();
}


void LiteChat_InviteFriend::on_pushButton_2_clicked()
{
    this->close();
}


void LiteChat_InviteFriend::on_pushButton_pressed()
{
    ui->pushButton->setStyleSheet("border-radius:10px;background-color: rgb(240, 240, 240);border-image: url(:/img/minus.png);");
}


void LiteChat_InviteFriend::on_pushButton_released()
{
    ui->pushButton->setStyleSheet("border-radius:10px;background-color: rgb(255, 255, 255);border-image: url(:/img/minus.png);");
}


void LiteChat_InviteFriend::on_pushButton_2_pressed()
{
    ui->pushButton_2->setStyleSheet("border-radius:10px;background-color: rgb(240, 240, 240);border-image: url(:/img/close.png);");
}


void LiteChat_InviteFriend::on_pushButton_2_released()
{
    ui->pushButton_2->setStyleSheet("border-radius:10px;background-color: rgb(255, 255, 255);border-image: url(:/img/close.png);");
}

