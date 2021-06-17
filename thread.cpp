#include "thread.h"
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonArray>

Thread::Thread(unsigned char new_p_id, QTcpSocket* newSocket, QObject *parent) : QObject(parent) {
    p_id = new_p_id;
    socket = newSocket;
    qDebug() << "connecting from " << socket->peerAddress().toString() << ":" << socket->peerPort();
    in.setDevice(socket);
    in.setVersion(QDataStream::Version::Qt_5_15);
    connect(socket, &QTcpSocket::readyRead, this, &Thread::recvMessage);
    connect(socket, &QTcpSocket::disconnected, this, &Thread::disconnected);
}

void Thread::disconnected() {
    qDebug() << "Disconnect " << p_id;
    emit closeThread(this);
}

void Thread::recvMessage() {
    QJsonObject json;
    in >> json;
    Command command(json);
    CommandType type = command.getType();
    qDebug() << "recv " << json;
    if(type == CommandType::SELECT || type == CommandType::UPDATE)
        emit execQuery(p_id, command);
    if(type == CommandType::REGISTER || type == CommandType::SIGNIN || type == CommandType::CHANGE_BALANCE ||
            type == CommandType::CHANGE_PASSWORD || type == CommandType::QUERY_BALANCE ||
            type == CommandType::SIGNOUT || type == CommandType::QUERY_PERCENT)
        emit execUser(p_id, command);
    if(type == CommandType::QUERY_EVENT)
        emit execEvent(p_id, command);
    if(type == CommandType::CART_SELECT || type == CommandType::CART_UPDATE || type == CommandType::CART_DEAL ||
            type == CommandType::CART_DELETE)
        emit execCart(p_id, command);
    if(type == CommandType::ORDER_SELECT || type == CommandType::ORDER_UPDATE || type == CommandType::ORDER_DELETE ||
            type == CommandType::ORDER_FINISH)
        emit execOrder(p_id, command);
}

void Thread::sendMessage(QJsonArray json) {
    qDebug() << "send " << json;
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << json;
    socket->write(block);
}

void Thread::sendInfoMessage(QJsonObject msg) {
    qDebug() << "send " << msg;
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << msg;
    socket->write(block);
}

BaseUser* Thread::getCurrentUser() {
    return currentUser;
}

void Thread::login(BaseUser *user) {
    currentUser = user;
}

void Thread::signout() {
    currentUser = nullptr;
}

unsigned char Thread::get_id() {
    return p_id;
}
