#include "cartcommodity.h"
#include <QVariant>
#include <QDebug>

CartCommodity::CartCommodity(QString n_cur, QString n_name, QString n_userid, int n_num){
    cur = n_cur;
    name = n_name;
    userid = n_userid;
    num = n_num;
    selected = false;
}

CartCommodity::CartCommodity(QJsonObject json) {
    name = json["name"].toString();
    userid = json["userid"].toString();
    cur = json["cur"].toString();
    num = json["num"].toInt();
    selected = json["selected"].toBool();
}

QJsonObject CartCommodity::toJsonObject() {
    QJsonObject json;
    json["name"] = name;
    json["userid"] = userid;
    json["cur"] = cur;
    json["num"] = num;
    json["selected"] = selected;
    return json;
}

QString CartCommodity::getName() {
    return name;
}

QString CartCommodity::getUserID() {
    return userid;
}

bool CartCommodity::getSelected() {
    return selected;
}

int CartCommodity::getNum() {
    return num;
}

int CartCommodity::getStore() {
    QString queryStr = "SELECT * FROM Commodity WHERE Name='%1' AND User='%2'";
    QSqlQuery query;
    query.exec(queryStr.arg(name).arg(userid));
    if(query.next())
        return query.value(5).toString().toInt();
    else
        return 0;
}

float CartCommodity::getPrice() {
    float price = 0;
    QString type;
    float percent = 1.0;
    QString queryStr = "SELECT * FROM Commodity Where Name='%1' AND User='%2'";
    QSqlQuery query;

    query.exec(queryStr.arg(name).arg(userid));
    if(query.next()) {
        type = query.value(2).toString();
        price = query.value(4).toString().toFloat();
    }

    queryStr = "SELECT * FROM Event Where User = '%1' AND Type = '%2'";
    query.exec(queryStr.arg(userid).arg(type));
    if(query.next())
        percent = query.value(2).toString().toFloat();

    return price * percent;
}

void CartCommodity::changeSelect() {
    selected = !selected;
}

void CartCommodity::deleteFromCart() {
    QSqlQuery query;
    QString queryStr = "DELETE FROM %1_Cart WHERE Name='%2' AND User='%3'";
    query.exec(queryStr.arg(cur).arg(name).arg(userid));
}

void CartCommodity::finishFromCart() {
    QSqlQuery query;
    QString queryStr = "UPDATE Commodity SET Store=Store-%1 WHERE Name='%2' AND User='%3'";
    query.exec(queryStr.arg(num).arg(name).arg(userid));

}

void CartCommodity::cancel() {
    QSqlQuery query;
    QString queryStr = "UPDATE Commodity SET Store=Store+%1 WHERE Name='%2' AND User='%3'";
    query.exec(queryStr.arg(num).arg(name).arg(userid));
}
