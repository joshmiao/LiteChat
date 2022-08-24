#include "litechat_invitefriend.h"
#include "ui_litechat_invitefriend.h"

LiteChat_InviteFriend::LiteChat_InviteFriend(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LiteChat_InviteFriend)
{
    ui->setupUi(this);
}

LiteChat_InviteFriend::~LiteChat_InviteFriend()
{
    delete ui;
}
