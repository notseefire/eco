#include "user.h"

BaseUser::BaseUser(QString& userid, QString& password)
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

SellerUser::SellerUser(QString& n_userid, QString& n_password, float n_money)
    : BaseUser(n_userid, n_password) {
    money = n_money;
}

void SellerUser::addMoney(float price) {
    money += price;
}

float SellerUser::getMoney() {
    return money;
}

UserType SellerUser::getUserType() const {
    return UserType::Seller;
}

CustomerUser::CustomerUser(QString& n_userid, QString& n_password, float n_balance)
    : BaseUser(n_userid, n_password) {
    // 初始用户没有任何余额
    balance = n_balance;
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
