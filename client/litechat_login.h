#ifndef LITECHAT_LOGIN_H
#define LITECHAT_LOGIN_H

#include <QMainWindow>
#include <QPoint>



namespace Ui {
class LiteChat_Login;
}

class LiteChat_Server;

class LiteChat_Login : public QMainWindow
{
    Q_OBJECT

public:
    explicit LiteChat_Login(LiteChat_Server *liteChatServer, QWidget *parent = nullptr);
    ~LiteChat_Login();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);


public slots:
    void loginSuccess(QString loginName, int32_t loginId);

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::LiteChat_Login *ui;
    LiteChat_Server *liteChatServer;
    QPoint m_mousePoint;
    QPoint movePoint;
    bool mousePress;
};

#endif // LITECHAT_LOGIN_H
