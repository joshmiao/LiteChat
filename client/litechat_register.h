#ifndef LITECHAT_REGISTER_H
#define LITECHAT_REGISTER_H

#include <QMainWindow>

namespace Ui {
class LiteChat_Register;
}

class LiteChat_Server;

class LiteChat_Register : public QMainWindow
{
    Q_OBJECT

public:
    explicit LiteChat_Register(LiteChat_Server *liteChatServer ,QWidget *parent = nullptr);
    ~LiteChat_Register();

private:
    Ui::LiteChat_Register *ui;
    LiteChat_Server *liteChatServer;
};

#endif // LITECHAT_REGISTER_H
