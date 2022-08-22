#include "litechat_interface.h"
#include "ui_litechat_interface.h"

LiteChat_Interface::LiteChat_Interface(LiteChat_Server *liteChatServer, QString loginName, int32_t loginId, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LiteChat_Interface),
    liteChatServer(liteChatServer),
    userinfo(loginId, loginName)
{
    ui->setupUi(this);
    connect(ui->listWidget, &QListWidget::itemClicked, this, &LiteChat_Interface::changeCurrentDialog);
    currentDialog = nullptr;
    addSingleChatListItem(LiteChat_Dialog::Private, 10002, "测试私聊10002");
    addSingleChatListItem(LiteChat_Dialog::Private, 10003, "测试私聊10003");
}

LiteChat_Interface::~LiteChat_Interface()
{
    delete ui;
}

LiteChat_DialogListItem::LiteChat_DialogListItem(LiteChat_Dialog::Dialog_Type dialogType, int32_t toId, QString chatName, QWidget *parent) :
    QWidget(parent),
    dialogType(dialogType),
    toId(toId),
    chatName(chatName),
    chatNameLabel(new QLabel(chatName, this)),
    chatContentLabel(new QLabel("ChatContent", this))
{
    QFont font;
    font.setPointSize(9);
    resize(QSize(parent->size().width(), 60));
    chatNameLabel->setFont(font);
    chatNameLabel->setAttribute(Qt::WA_TranslucentBackground);
    chatNameLabel->setGeometry(60, 5, parent->size().width() - 40, 20);
    chatContentLabel->setFont(font);
    chatContentLabel->setAttribute(Qt::WA_TranslucentBackground);
    chatContentLabel->setGeometry(60, 25, parent->size().width() - 40, 20);
}

void LiteChat_Interface::changeCurrentDialog(QListWidgetItem *currentItem)
{
    Q_UNUSED(currentItem);
    qDebug() << "####" << ui->listWidget->currentRow() << '\n';
    LiteChat_DialogListItem *currentFriend = dialogList[ui->listWidget->currentRow()];
    LiteChat_Dialog::Dialog_Type dialogType = currentFriend->dialogType;
    int32_t toId = currentFriend->toId;
    QString chatName = currentFriend->chatName;
    if (currentDialog != nullptr && dialogType == currentDialog->dialogType && toId == currentDialog->toId) return;

    if (currentDialog != nullptr){
        ui->horizontalLayout->removeWidget(currentDialog);
        currentDialog->hide();
    }
    auto iter = openedDialog.find({dialogType, toId});
    if (iter != openedDialog.end()) {
        currentDialog = iter->second;
        ui->horizontalLayout->addWidget(currentDialog);
        currentDialog->show();
    }
    else{
        currentDialog = liteChatServer->createDialog(chatName, dialogType, toId);
        openedDialog[{dialogType, toId}] = currentDialog;
        ui->horizontalLayout->addWidget(currentDialog);
        currentDialog->show();
    }

    return;
}

void LiteChat_Interface::addSingleChatListItem(LiteChat_Dialog::Dialog_Type dialogType, int32_t toId, QString chatName)
{
    LiteChat_DialogListItem *newFriend = new LiteChat_DialogListItem(dialogType, toId, chatName, ui->listWidget);
    qDebug() << "****" << ui->listWidget->count() << '\n';
    QListWidgetItem *newItem = new QListWidgetItem(ui->listWidget);
    newItem->setSizeHint(QSize(ui->listWidget->size().width() - 10, 60));
    ui->listWidget->setItemWidget(newItem, newFriend);
    dialogList[ui->listWidget->count() - 1] = newFriend;
}
