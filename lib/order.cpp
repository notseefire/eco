#include "order.h"
#include <QJsonArray>

Order::Order(QString n_cur, int n_num, float n_price){
    cur = n_cur;
    num = n_num;
    price = n_price;
}

Order::Order(QJsonObject json) {
    cur = json["cur"].toString();
    num = json["num"].toInt();
    price = json["price"].toDouble();
    orderList.clear();
    QJsonArray array = json["orderList"].toArray();
    for(auto index = array.begin(); index != array.end(); index++) {
        QJsonObject json = (*index).toObject();
        orderList.append(new CartCommodity(json));
    }
}

QJsonObject Order::toJsonObject() {
    QJsonObject json;
    json["cur"] = cur;
    json["num"] = num;
    json["price"] = price;
    QJsonArray array;
    for(auto index = orderList.begin(); index != orderList.end(); index++) {
        array.append((*index)->toJsonObject());
    }
    json["orderList"] = array;
    return json;
}

int Order::getID() {
    return num;
}

float Order::getPrice() {
    return price;
}

QList<CartCommodity*> Order::getList() {
    return orderList;
}

void Order::addCommodity(QString name, QString userid, int num) {
    orderList.append(new CartCommodity(cur, name, userid, num));
}

