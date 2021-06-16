#ifndef THREAD_H
#define THREAD_H

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include "command.h"
#include "../../eco/eco/lib/user.h"

class Thread : public QObject
{
    Q_OBJECT
public:
    explicit Thread(unsigned char new_p_id, QTcpSocket* newSocket, QObject *parent = nullptr);
    unsigned char get_id();
    void sendMessage(QJsonArray json);
    void sendInfoMessage(QJsonObject msg);
    BaseUser* getCurrentUser();
    void login(BaseUser* user);
    void signout();

    bool operator == (Thread &rhs) {
        return p_id == rhs.p_id;
    }
signals:
    void execQuery(unsigned char p_id, Command command);
    void execUser(unsigned char p_id, Command command);
    void execEvent(unsigned char p_id, Command command);
    void closeThread(Thread* thread);

private slots:
    void recvMessage();
    void disconnected();

private:
    unsigned char p_id;
    QTcpSocket *socket;
    QDataStream in;
    BaseUser* currentUser;

};

#endif // THREAD_H
