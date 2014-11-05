// Copyright (c) 2014 Cesanta Software
// All rights reserved

#include "mongoose.h"
#include "mainwindow.h"
#include <QApplication>
#include <RestfulApi.h>


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qDebug()<<"Main Society Pro: "<<QThread::currentThreadId();

    RestfulApi webServerThread;

    // destroy the thread before quit the app
    QObject::connect(&a, SIGNAL(aboutToQuit()), &webServerThread, SLOT(sl_quit()));
    //start the webserver thread
    webServerThread.start();
    //The gui must be called after the server loads...
     MainWindow w;
   return a.exec();
}

