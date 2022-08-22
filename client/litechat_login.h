#ifndef LITECHAT_LOGIN_H
#define LITECHAT_LOGIN_H

#include <QMainWindow>


namespace Ui {
class LiteChat_Login;
}

class LiteChat;

class LiteChat_Login : public QMainWindow
{
    Q_OBJECT

public:
    explicit LiteChat_Login(LiteChat *liteChatMain, QWidget *parent = nullptr);
    ~LiteChat_Login();

public slots:
    void loginSuccess(QString loginName, int32_t loginId);

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

private:
    Ui::LiteChat_Login *ui;
    LiteChat *liteChatMain;
};

#endif // LITECHAT_LOGIN_H
