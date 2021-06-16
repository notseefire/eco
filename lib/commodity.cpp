#include "commodity.h"
#include <QSqlQuery>
#include <QVariant>

BaseCommodity::BaseCommodity(QString name, QString userid, QString description, float price, int store):
    name(name), userid(userid), description(description), price(price), store(store) {}

BaseCommodity::BaseCommodity(QJsonObject& json) {
    name = json["name"].toString();
    userid = json["userid"].toString();
    description = json["description"].toString();
    price = json["price"].toDouble();
    store = json["store"].toInt();
}

QJsonObject BaseCommodity::toJsonObject() {
    QJsonObject json;
    json["name"] = name;
    json["userid"] = userid;
    json["description"] = description;
    json["price"] = price;
    json["store"] = store;
    json["type"] = getType();
    return json;
}

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

ClothCommodity::ClothCommodity(QJsonObject& json)
    : BaseCommodity(json) {}

BookCommodity::BookCommodity(QString name, QString userid, QString description, float price, int store)
    : BaseCommodity(name, userid, description, price, store) {}

BookCommodity::BookCommodity(QJsonObject& json)
    : BaseCommodity(json) {}

FoodCommodity::FoodCommodity(QString name, QString userid, QString description, float price, int store)
    : BaseCommodity(name, userid, description, price, store) {}

FoodCommodity::FoodCommodity(QJsonObject& json)
    : BaseCommodity(json) {}

float ClothCommodity::getActualPrice() const {
    QString queryStr = "SELECT * FROM Event Where User = '%1' AND Type = 'Cloth'";
    QSqlQuery query;
    query.exec(queryStr.arg(getUserID()));
    if(query.next()) {
        float percent = query.value(2).toString().toFloat();
        return percent;
    }
    else {
        return 1.0;
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
        return percent;
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
        return percent;
    }
    else {
        return getPrice();
    }
}

QString FoodCommodity::getType() const {
    return "Food";
}
