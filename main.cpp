#include "ide.h"
#include <BerryMath.h>
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    IDE w;
    w.show();

    return a.exec();
}
