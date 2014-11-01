#include "mainwindow.h"
#include <QApplication>
#include <RestfulApi.h>

int main(int argc, char *argv[])
{



    QApplication a(argc, argv);
    qDebug()<<"Main Society Pro: "<<QThread::currentThreadId();
    RestfulApi thread;
    QObject::connect(&thread, SIGNAL(finished()), &a, SLOT(deleteLater()));
    thread.start();

    //The gui must be called after the server loads...
     MainWindow w;
    return a.exec();
}
