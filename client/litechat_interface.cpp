#include "litechat_finduser.h"
#include "litechat_interface.h"
#include "ui_litechat_interface.h"

DialogInfo::DialogInfo(LiteChat_Dialog::Dialog_Type dialogType, int32_t toId, QString dialogName, QDateTime lastMessageTime, QString lastMessage):
    dialogType(dialogType),
    toId(toId),
    dialogName(dialogName),
    lastMessageTime(lastMessageTime),
    lastMessage(lastMessage)
{

}

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

LiteChat_DialogListItem::LiteChat_DialogListItem(LiteChat_Dialog::Dialog_Type dialogType, int32_t toId, QString dialogName, QString lastMessage, QWidget *parent) :
    QWidget(parent),
    dialogType(dialogType),
    toId(toId),
    dialogName(dialogName),
    lastMessage(lastMessage)
{
    QString str1 = dialogName.replace('\n', ""), str2 = lastMessage.replace('\n', "");
    if (str1.length() > 15) str1 = str1.left(12) + "...";
    if (str2.length() > 18) str2 = str2.left(15) + "...";
    dialogNameLabel = new QLabel(str1, this);
    dialogContentLabel = new QLabel(str2, this);
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
    currentRow = mappingIndex[currentRow];
    qDebug() << "####" << currentRow << '\n';
    LiteChat_Dialog::Dialog_Type dialogType = dialogInfoList[currentRow].dialogType;
    int32_t toId = dialogInfoList[currentRow].toId;
    QString dialogName = dialogInfoList[currentRow].dialogName;
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
    dialogInfoList.push_back(DialogInfo(dialogType, toId, dialogName, QDateTime::currentDateTime(), ""));
    flushDialogList();
    liteChatServer->requestMessages(toId);
}

void LiteChat_Interface::deleteSingleDialogListItem(LiteChat_Dialog::Dialog_Type dialogType, int32_t toId)
{
    if (!dialogListIndex.count({dialogType, toId})) return;
    for (auto iter = dialogInfoList.begin(); iter != dialogInfoList.end(); ++iter)
    if ((iter->dialogType == dialogType) && (iter->toId == toId))
    {
         dialogInfoList.erase(iter);
         break;
    }
    flushDialogList();
    currentDialog = nullptr;
    openedDialog.erase({dialogType, toId});
}

void LiteChat_Interface::messageReceive(LiteChat_Dialog::Dialog_Type dialogType, int32_t fromId, int32_t toId, QString msg){
    if (fromId == userinfo.id){
        if (!dialogListIndex.count({dialogType, toId})){
            qDebug() << "Message recieved from stranger.\n";
            return;
        }
        dialogInfoList[dialogListIndex[{dialogType, toId}]].lastMessage = msg;
        dialogInfoList[dialogListIndex[{dialogType, toId}]].lastMessageTime = QDateTime::currentDateTime();
        flushDialogList();
        auto iter = openedDialog.find({dialogType, toId});
        if (iter != openedDialog.end()) {
            iter->second->receiveSingalMessage(msg, true);
        }
        else{
            QString dialogName = dialogInfoList[dialogListIndex[{dialogType, toId}]].dialogName;
            LiteChat_Dialog *newDialog = liteChatServer->createDialog(dialogName, dialogType, toId);
            openedDialog[{dialogType, toId}] = newDialog;
            newDialog->receiveSingalMessage(msg, true);
        }
        return;
    }


    if (!dialogListIndex.count({dialogType, fromId})){
        qDebug() << "Message recieved from stranger.\n";
        return;
    }
    dialogInfoList[dialogListIndex[{dialogType, fromId}]].lastMessage = msg;
    dialogInfoList[dialogListIndex[{dialogType, fromId}]].lastMessageTime = QDateTime::currentDateTime();
    flushDialogList();
    auto iter = openedDialog.find({dialogType, fromId});
    if (iter != openedDialog.end()) {
        iter->second->receiveSingalMessage(msg, false);
    }
    else{
        QString dialogName = dialogInfoList[dialogListIndex[{dialogType, fromId}]].dialogName;
        LiteChat_Dialog *newDialog = liteChatServer->createDialog(dialogName, dialogType, fromId);
        openedDialog[{dialogType, fromId}] = newDialog;
        newDialog->receiveSingalMessage(msg, false);
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

void LiteChat_Interface::flushDialogList(){
    std::sort(dialogInfoList.begin(), dialogInfoList.end());
    disconnect(ui->listWidget, &QListWidget::currentRowChanged, this, &LiteChat_Interface::changeCurrentDialog);
    ui->listWidget->clear();
    dialogListIndex.clear();
    mappingIndex.clear();
    for (uint32_t i = 0; i < dialogInfoList.size(); ++i){
        DialogInfo &info = dialogInfoList[i];
        dialogListIndex[{info.dialogType, info.toId}] = i;
        if (ui->lineEdit->text() == "" || ui->lineEdit->text() == QString::fromStdString(std::to_string(info.toId)) || ui->lineEdit->text() == info.dialogName)
        {
            LiteChat_DialogListItem *newFriend = new LiteChat_DialogListItem(info.dialogType, info.toId, info.dialogName, info.lastMessage, ui->listWidget);
            mappingIndex.push_back(i);
            QListWidgetItem *newItem = new QListWidgetItem(ui->listWidget);
            newItem->setSizeHint(QSize(ui->listWidget->size().width() - 10, 60));
            ui->listWidget->setItemWidget(newItem, newFriend);
        }
    }
    connect(ui->listWidget, &QListWidget::currentRowChanged, this, &LiteChat_Interface::changeCurrentDialog);
}



void LiteChat_Interface::on_lineEdit_textChanged(const QString &arg1)
{
    Q_UNUSED(arg1);
    flushDialogList();
}


void LiteChat_Interface::on_pushButton_3_clicked()
{

}

