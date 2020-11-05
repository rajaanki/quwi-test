#ifndef UWIMAIN_H
#define UWIMAIN_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class UwiMain; }
QT_END_NAMESPACE

class UwiMain : public QMainWindow
{
    Q_OBJECT

public:
    UwiMain(QWidget *parent = nullptr);
    ~UwiMain();

private:
    Ui::UwiMain *ui;
};
#endif // UWIMAIN_H
