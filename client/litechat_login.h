#ifndef LITECHAT_LOGIN_H
#define LITECHAT_LOGIN_H

#include <QMainWindow>
#include "litechat.h"

namespace Ui {
class LiteChat_Login;
}

class LiteChat_Login : public QMainWindow
{
    Q_OBJECT

public:
    explicit LiteChat_Login(LiteChat *liteChatMain, QWidget *parent = nullptr);
    ~LiteChat_Login();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::LiteChat_Login *ui;
    LiteChat *liteChatMain;
};

#endif // LITECHAT_LOGIN_H
