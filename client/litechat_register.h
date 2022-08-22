#ifndef LITECHAT_REGISTER_H
#define LITECHAT_REGISTER_H

#include <QMainWindow>

namespace Ui {
class LiteChat_Register;
}

class LiteChat;

class LiteChat_Register : public QMainWindow
{
    Q_OBJECT

public:
    explicit LiteChat_Register(LiteChat *liteChatMain ,QWidget *parent = nullptr);
    ~LiteChat_Register();

private:
    Ui::LiteChat_Register *ui;
    LiteChat *liteChatMain;
};

#endif // LITECHAT_REGISTER_H
