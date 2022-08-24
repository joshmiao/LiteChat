#ifndef LITECHAT_REGISTER_H
#define LITECHAT_REGISTER_H

#include <QMainWindow>
#include <QPoint>


namespace Ui {
class LiteChat_Register;
}

class LiteChat_Server;

class LiteChat_Register : public QMainWindow
{
    Q_OBJECT

public slots:
    void registerSuccess(int32_t id);

public:
    explicit LiteChat_Register(LiteChat_Server *liteChatServer ,QWidget *parent = nullptr);
    ~LiteChat_Register();
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);


private slots:
    void on_pushButton_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::LiteChat_Register *ui;
    LiteChat_Server *liteChatServer;
    QPoint m_mousePoint;
    QPoint movePoint;
    bool mousePress;
};

#endif // LITECHAT_REGISTER_H
