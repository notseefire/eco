#ifndef ORDER_H
#define ORDER_H

#include <QString>
#include <QSqlQuery>
#include <QList>
#include "./lib/cartcommodity.h"

class Order
{
public:
    Order(QString n_cur, int num, float price);
    void completeOrder();
    int getID();
    float getPrice();
    void addCommodity(QString name, QString userid, int num);
    QList<CartCommodity*> getList();

private:
    QString cur;
    int num;
    float price;
    QList<CartCommodity*> orderList;
};

#endif // ORDER_H
