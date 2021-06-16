#ifndef COMMAND_H
#define COMMAND_H

#include <QJsonObject>
#include "../../eco/eco/lib/commodity.h"

enum CommandType {
    SELECT,
    UPDATE,
    SIGNIN,
    REGISTER,
    QUERY_BALANCE,
    CHANGE_PASSWORD,
    CHANGE_BALANCE,
    SIGNOUT,
    QUERY_PERCENT,
    QUERY_EVENT,
};

enum SqlResponse {
    SUCCESS,
    ERROR,
};

struct SqlUpdateResult {
    SqlResponse reponse;
    QString text;
};

class Command
{
public:
    Command(CommandType n_type);
    Command(CommandType n_type, QString n_query);
    Command(CommandType n_type, QString n_userid, QString n_commodity_type, float n_percent);
    Command(CommandType n_type, float n_balance);
    Command(CommandType n_type, QString n_userid, QString n_password);
    Command(CommandType n_type, QString n_userid, QString n_password, int n_usertype);
    Command(CommandType n_type, QJsonObject n_commodity);
    Command(QJsonObject &json);

    QString getQuery();
    CommandType getType();
    QString userid;
    QString password;
    QString commodity_type;
    int usertype = -1;
    float balance = 0;
    float percent = 0;
    QJsonObject commodity;

    QJsonObject toJsonObject();

private:
    CommandType type;
    QString query;
};

#endif // COMMAND_H
