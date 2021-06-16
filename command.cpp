#include "command.h"

Command::Command(CommandType n_type) {
    type = n_type;
}

Command::Command(CommandType n_type, QString n_query) {
    type = n_type;
    query = n_query;
}

Command::Command(CommandType n_type, QString n_userid, QString n_password) {
    type = n_type;
    userid = n_userid;
    password = n_password;
}
Command::Command(CommandType n_type, QString n_userid, QString n_password, int n_usertype) {
    type = n_type;
    userid = n_userid;
    password = n_password;
    usertype = n_usertype;
}

Command::Command(CommandType n_type, float n_balance) {
    type = n_type;
    balance = n_balance;
}

Command::Command(CommandType n_type, QJsonObject n_commodity) {
    type = n_type;
    commodity = n_commodity;
}

Command::Command(CommandType n_type, QString n_userid, QString n_commodity_type, float n_percent) {
    type = n_type;
    userid = n_userid;
    commodity_type = n_commodity_type;
    percent = n_percent;
}

Command::Command(QJsonObject &json) {
    int type_num = json["type"].toInt();
    switch (type_num) {
    case CommandType::SELECT:
        type = CommandType::SELECT;
        break;
    case CommandType::UPDATE:
        type = CommandType::UPDATE;
        break;
    case CommandType::REGISTER:
        type = CommandType::REGISTER;
        break;
    case CommandType::SIGNIN:
        type = CommandType::SIGNIN;
        break;
    case CommandType::CHANGE_BALANCE:
        type = CommandType::CHANGE_BALANCE;
        break;
    case CommandType::CHANGE_PASSWORD:
        type = CommandType::CHANGE_PASSWORD;
        break;
    case CommandType::QUERY_BALANCE:
        type = CommandType::QUERY_BALANCE;
        break;
    case CommandType::SIGNOUT:
        type = CommandType::SIGNOUT;
        break;
    case CommandType::QUERY_PERCENT:
        type = CommandType::QUERY_PERCENT;
        break;
    case CommandType::QUERY_EVENT:
        type = CommandType::QUERY_EVENT;
        break;
    }

    if(json.contains("query")) {
        query = json["query"].toString();
    }
    if(json.contains("userid")) {
        userid = json["userid"].toString();
    }
    if(json.contains("password")) {
        password = json["password"].toString();
    }
    if(json.contains("usertype")) {
        usertype = json["usertype"].toInt();
    }
    if(json.contains("balance")) {
        balance = json["balance"].toDouble();
    }
    if(json.contains("commodity")){
        commodity = json["commodity"].toObject();
    }
    if(json.contains("commodity_type")) {
        commodity_type = json["commodity_type"].toString();
    }
    if(json.contains("percent")) {
        percent = json["percent"].toDouble();
    }
}

QJsonObject Command::toJsonObject() {
    QJsonObject json;
    json["type"] = type;
    if(!userid.isEmpty())
        json["userid"] = userid;
    if(!password.isEmpty())
        json["password"] = password;
    if(!query.isEmpty())
        json["query"] = query;
    if(balance != 0)
        json["balance"] = balance;
    if(usertype != -1)
        json["usertype"] = usertype;
    if(!commodity.isEmpty())
        json["commodity"] = commodity;
    if(!commodity_type.isEmpty())
        json["commodity_type"] = commodity_type;
    if(percent != 0.0)
        json["percent"] = percent;
    return json;
}

QString Command::getQuery() {
    return query;
}

CommandType Command::getType() {
    return type;
}
