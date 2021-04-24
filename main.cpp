#include "mainwindow.h"
#include "remote.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
//    Remote r;
//    r.initialize(&w);

//    a.setOverrideCursor(Qt::BlankCursor);

    w.show();
//    r.show();
    return a.exec();
}
