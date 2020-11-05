#include "uwihome.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UwiHome w;
    w.show();
    return a.exec();
}
