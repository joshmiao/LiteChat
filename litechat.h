#ifndef LITECHAT_H
#define LITECHAT_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class LiteChat; }
QT_END_NAMESPACE

class LiteChat : public QMainWindow
{
    Q_OBJECT

public:
    LiteChat(QWidget *parent = nullptr);
    ~LiteChat();

private:
    Ui::LiteChat *ui;
};
#endif // LITECHAT_H
