#ifndef LITECHAT_INTERFACE_H
#define LITECHAT_INTERFACE_H

#include <QMainWindow>

namespace Ui {
class LiteChat_Interface;
}

class LiteChat_Interface : public QMainWindow
{
    Q_OBJECT

public:
    explicit LiteChat_Interface(QWidget *parent = nullptr);
    ~LiteChat_Interface();

private:
    Ui::LiteChat_Interface *ui;
};

#endif // LITECHAT_INTERFACE_H
