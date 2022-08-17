#ifndef LITECHAT_H
#define LITECHAT_H

#include <QMainWindow>
#include <QTcpSocket>

QT_BEGIN_NAMESPACE
namespace Ui { class LiteChat; }
QT_END_NAMESPACE

class LiteChat : public QMainWindow
{
    Q_OBJECT

public:
    LiteChat(QWidget *parent = nullptr);
    ~LiteChat();

private slots:
    void on_pushButton_clicked();
    void handConnected();

private:
    Ui::LiteChat *ui;
    QTcpSocket *client;
};
#endif // LITECHAT_H
