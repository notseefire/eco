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
    CART_UPDATE,
    CART_SELECT,
    CART_DEAL,
    CART_DELETE,
    ORDER_UPDATE,
    ORDER_SELECT,
    ORDER_DELETE,
    ORDER_FINISH,
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
    Command(CommandType n_type, int n_num);
    Command(CommandType n_type, QString n_query);
    Command(CommandType n_type, QString n_userid, QString n_commodity_type, float n_percent);
    Command(CommandType n_type, float n_balance);
    Command(CommandType n_type, QString n_userid, QString n_password);
    Command(CommandType n_type, QString n_userid, QString n_password, int n_usertype);
    Command(CommandType n_type, QString n_name, QString n_userid, int n_num, QString n_cur);
    Command(CommandType n_type, QJsonObject n_commodity);
    Command(CommandType n_type, QJsonArray n_list);
    Command(QJsonObject &json);

    QString getQuery();
    CommandType getType();
    QString userid;
    QString password;
    QString commodity_type;
    QString cur;
    QString name;
    int usertype = -1;
    int num = 0;
    float balance = 0;
    float percent = 0;
    QJsonObject commodity;
    QJsonArray list;

    QJsonObject toJsonObject();

private:
    CommandType type;
    QString query;
};

#endif // COMMAND_H
