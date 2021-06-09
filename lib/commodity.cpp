#include "commodity.h"
#include <QSqlQuery>
#include <QVariant>

BaseCommodity::BaseCommodity(QString name, QString userid, QString description, float price, int store):
    name(name), userid(userid), description(description), price(price), store(store) {}

QString BaseCommodity::getName() const {
    return name;
}

QString BaseCommodity::getDescription() const {
    return description;
}

BaseCommodity::~BaseCommodity() {

}

void BaseCommodity::setDescription(QString n_description) {
    description = n_description;
}

float BaseCommodity::getPrice() const {
    return price;
}

void BaseCommodity::setPrice(float n_price){
    price = n_price;
}

int BaseCommodity::getStore() const {
    return store;
}

void BaseCommodity::setStore(int n_store) {
    store = n_store;
}

QString BaseCommodity::getUserID() const {
    return userid;
}

ClothCommodity::ClothCommodity(QString name, QString userid, QString description, float price, int store)
    : BaseCommodity(name, userid, description, price, store) {}

BookCommodity::BookCommodity(QString name, QString userid, QString description, float price, int store)
    : BaseCommodity(name, userid, description, price, store) {}

FoodCommodity::FoodCommodity(QString name, QString userid, QString description, float price, int store)
    : BaseCommodity(name, userid, description, price, store) {}

float ClothCommodity::getActualPrice() const {
    QString queryStr = "SELECT * FROM Event Where User = '%1' AND Type = 'Cloth'";
    QSqlQuery query;
    query.exec(queryStr.arg(getUserID()));
    if(query.next()) {
        float percent = query.value(2).toString().toFloat();
        return percent * getPrice();
    }
    else {
        return getPrice();
    }
}

QString ClothCommodity::getType() const {
    return "Cloth";
}

float BookCommodity::getActualPrice() const {
    QString queryStr = "SELECT * FROM Event Where User = '%1' AND Type = 'Book'";
    QSqlQuery query;
    query.exec(queryStr.arg(getUserID()));
    if(query.next()) {
        float percent = query.value(2).toString().toFloat();
        return percent * getPrice();
    }
    else {
        return getPrice();
    }
}

QString BookCommodity::getType() const {
    return "Book";
}

float FoodCommodity::getActualPrice() const {
    QString queryStr = "SELECT * FROM Event Where User = '%1' AND Type = 'Food'";
    QSqlQuery query;
    query.exec(queryStr.arg(getUserID()));
    if(query.next()) {
        float percent = query.value(2).toString().toFloat();
        return percent * getPrice();
    }
    else {
        return getPrice();
    }
}

QString FoodCommodity::getType() const {
    return "Food";
}
