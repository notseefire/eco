#include "server.h"
#include <QApplication>
#include <QDebug>
#include <QObject>


int main(int argc, char *argv[]) {
    QApplication* a = new QApplication(argc, argv);
    Server *server = new Server();
    qDebug() << "server start";
    server->show();
    QObject::connect(a, &QApplication::aboutToQuit, server, &Server::aboutToQuit);
    return a->exec();
}
