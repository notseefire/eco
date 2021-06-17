#include "user.h"

BaseUser::BaseUser(QString userid, QString password)
    : userid(userid), password(password) {}

void BaseUser::setUserId(QString& new_userid) {
    userid = new_userid;
}

void BaseUser::setPassword(QString & new_password) {
    password = new_password;
}

QString BaseUser::getUserId() const {
    return userid;
}

QString BaseUser::getPassword() const {
    return password;
}

SellerUser::SellerUser(QString& n_userid, QString& n_password, float balance)
    : BaseUser(n_userid, n_password) {
    money = balance;
}

SellerUser::SellerUser(QJsonObject& json)
    : BaseUser(json["userid"].toString(), json["password"].toString()) {
    money = json["balance"].toDouble();
}

UserType SellerUser::getUserType() const {
    return UserType::Seller;
}


void SellerUser::addMoney(float price) {
    money += price;
}

float SellerUser::getMoney() {
    return money;
}


CustomerUser::CustomerUser(QString& n_userid, QString& n_password, float n_balance)
    : BaseUser(n_userid, n_password) {
    // 初始用户没有任何余额
    balance = n_balance;
}

CustomerUser::CustomerUser(QJsonObject& json)
    : BaseUser(json["userid"].toString(), json["password"].toString()) {
    balance = json["balance"].toDouble();
    int ibalance = balance * 100.0;
    balance = ibalance / 100.0;
}

UserType CustomerUser::getUserType() const {
    return UserType::Customer;
}

void CustomerUser::recharge(float addBalance) {
    balance += addBalance;
}

void CustomerUser::pay(float minusBalance) {
    balance -= minusBalance;
}

float CustomerUser::queryBalance() const {
    return balance;
}

QJsonObject CustomerUser::toJsonObject() const {
    QJsonObject json;
    json["usertype"] = UserType::Customer;
    json["userid"] = userid;
    json["password"] = password;
    json["balance"] = balance;
    return json;
}

QJsonObject SellerUser::toJsonObject() const {
    QJsonObject json;
    json["usertype"] = UserType::Seller;
    json["userid"] = userid;
    json["password"] = password;
    json["balance"] = money;
    return json;
}
