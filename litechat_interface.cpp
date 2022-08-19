#include "litechat_interface.h"
#include "ui_litechat_interface.h"

LiteChat_Interface::LiteChat_Interface(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::LiteChat_Interface)
{
    ui->setupUi(this);
}

LiteChat_Interface::~LiteChat_Interface()
{
    delete ui;
}
