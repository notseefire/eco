#ifndef CARTCOMMODITY_H
#define CARTCOMMODITY_H

#include <QString>
#include <QSqlQuery>
#include <QJsonObject>


class CartCommodity
{
public:
    CartCommodity(QString n_cur, QString n_name, QString n_userid, int n_num);
    CartCommodity(QJsonObject json);

    int getStore();
    float getPrice();
    QString getName();
    QString getUserID();
    bool getSelected();
    void changeSelect();
    int getNum();
    void deleteFromCart();
    void finishFromCart();
    void cancel();
    QJsonObject toJsonObject();

private:
    QString name;
    QString userid;
    QString cur;
    int num;
    bool selected;
};

#endif // CARTCOMMODITY_H
