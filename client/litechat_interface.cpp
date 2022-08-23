#include "litechat_finduser.h"
#include "litechat_interface.h"
#include "ui_litechat_interface.h"

LiteChat_Interface::LiteChat_Interface(LiteChat_Server *liteChatServer, QString loginName, int32_t loginId, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LiteChat_Interface),
    liteChatServer(liteChatServer),
    userinfo(loginId, loginName)
{
    ui->setupUi(this);
    connect(ui->listWidget, &QListWidget::currentRowChanged, this, &LiteChat_Interface::changeCurrentDialog);
    currentDialog = nullptr;
}

LiteChat_Interface::~LiteChat_Interface()
{
    delete ui;
}

LiteChat_DialogListItem::LiteChat_DialogListItem(LiteChat_Dialog::Dialog_Type dialogType, int32_t toId, QString dialogName, QWidget *parent) :
    QWidget(parent),
    dialogType(dialogType),
    toId(toId),
    dialogName(dialogName),
    dialogNameLabel(new QLabel(dialogName, this)),
    dialogContentLabel(new QLabel("ChatContent", this))
{
    QFont font;
    font.setPointSize(9);
    resize(QSize(parent->size().width(), 60));
    dialogNameLabel->setFont(font);
    dialogNameLabel->setAttribute(Qt::WA_TranslucentBackground);
    dialogNameLabel->setGeometry(60, 5, parent->size().width() - 40, 20);
    dialogContentLabel->setFont(font);
    dialogContentLabel->setAttribute(Qt::WA_TranslucentBackground);
    dialogContentLabel->setGeometry(60, 25, parent->size().width() - 40, 20);
}

void LiteChat_Interface::changeCurrentDialog(int currentRow)
{
    qDebug() << "####" << currentRow << '\n';
    LiteChat_DialogListItem *currentFriend = dialogList[currentRow];
    LiteChat_Dialog::Dialog_Type dialogType = currentFriend->dialogType;
    int32_t toId = currentFriend->toId;
    QString dialogName = currentFriend->dialogName;
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
        currentDialog = liteChatServer->createDialog(dialogName, dialogType, toId);
        openedDialog[{dialogType, toId}] = currentDialog;
        ui->horizontalLayout->addWidget(currentDialog);
        currentDialog->show();
    }

    return;
}

void LiteChat_Interface::addSingleDialogListItem(LiteChat_Dialog::Dialog_Type dialogType, int32_t toId, QString dialogName)
{
    if (dialogListIndex.count({dialogType, toId})) return;
    liteChatServer->requestMessages(toId);

    LiteChat_DialogListItem *newFriend = new LiteChat_DialogListItem(dialogType, toId, dialogName, ui->listWidget);
    dialogList[dialogList.size()] = newFriend;
    dialogListIndex[{dialogType, toId}] = dialogList.size() - 1;

    QListWidgetItem *newItem = new QListWidgetItem(ui->listWidget);
    newItem->setSizeHint(QSize(ui->listWidget->size().width() - 10, 60));
    ui->listWidget->setItemWidget(newItem, newFriend);

}

void LiteChat_Interface::messageReceive(LiteChat_Dialog::Dialog_Type dialogType, int32_t fromId, QString msg){
    if (!dialogListIndex.count({dialogType, fromId})){
        qDebug() << "Message recieved from stranger.\n";
        return;
    }
    auto iter = openedDialog.find({dialogType, fromId});
    if (iter != openedDialog.end()) {
        iter->second->receiveSingalMessage(msg);
    }
    else{
        QString dialogName = dialogList[dialogListIndex[{dialogType, fromId}]]->dialogName;
        LiteChat_Dialog *newDialog = liteChatServer->createDialog(dialogName, dialogType, fromId);
        openedDialog[{dialogType, fromId}] = newDialog;
        newDialog->receiveSingalMessage(msg);
    }
}

void LiteChat_Interface::on_pushButton_clicked()
{

}


void LiteChat_Interface::on_pushButton_2_clicked()
{
    LiteChat_FindUser *findUserPage = liteChatServer->createFindUser();
    findUserPage->show();
}



