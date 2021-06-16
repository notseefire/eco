#include "order.h"

Order::Order(QString n_cur, int n_num, float n_price){
    cur = n_cur;
    num = n_num;
    price = n_price;
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
