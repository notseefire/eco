#include "client.h"
#include <QFile>
#include <QTextStream>
#include <QDebug>
#include <QDir>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QSqlError>

#define DEBUG

Client::Client(QString filename, QObject* parent): QObject(parent) {
    configFile = filename;
    currentUser = nullptr;
    try{
        loadUserConfig(configFile);
        openDataBase();
    } catch(const char* msg) {
        qDebug() << msg << Qt::endl;
        exit(0);
    } catch(QSqlError sqlerr) {
        qDebug() << "创建数据库时发生错误";
        qDebug() << sqlerr.text() << Qt::endl;
        exit(0);
    }


}

Client::~Client() {
    saveUserConfig(configFile);
}

void Client::loginUser(QString userid, QString password) {
    if(!userid.length()){
        emit errorHappen("用户名不能为空");
        return;
    }

    if(!password.length()){
        emit errorHappen("密码不能为空");
        return;
    }
    for(auto index = customerList.begin(); index != customerList.end(); index++) {
        BaseUser* baseUser = *index;
        if (baseUser->userid == userid) {
            if (baseUser->password == password) {
                currentUser = baseUser;
                curCustomerUser = *index;
                emit signIn(true);
                emit balanceChange(curCustomerUser->queryBalance());
                return;
            }
        }
    }

    for(auto index = sellerList.begin(); index != sellerList.end(); index++) {
        BaseUser* baseUser = *index;
        if (baseUser->userid == userid) {
            if (baseUser->password == password) {
                currentUser = baseUser;
                curSellerUser = *index;
                emit signIn(false);
                return;
            }
        }
    }

    emit errorHappen("用户名不存在或者密码错误");
}

bool Client::is_login() const {
    return currentUser != nullptr;
}

QString Client::getUserId() const {
    return currentUser->getUserId();
}


bool Client::registerUser(int userType, QString userid, QString password, float balance) {
    for(auto index = customerList.begin(); index != customerList.end(); index++) {
        BaseUser* baseUser = *index;
        if (baseUser->userid == userid) {
            emit errorHappen("用户名已存在");
            return false;
        }
    }

    for(auto index = sellerList.begin(); index != sellerList.end(); index++) {
        BaseUser* baseUser = *index;
        if (baseUser->userid == userid) {
            qDebug() << "Register Error" << Qt::endl;
            emit errorHappen("用户名已存在");
            return false;
        }
    }

    // 0 for Customer, 1 for Seller
    switch (userType) {
        case 0:
            customerList.append(new CustomerUser(userid, password, balance));
        break;
        case 1:
            sellerList.append(new SellerUser(userid, password));
        break;
    }
    emit infoHappen("注册成功");
    return true;
}

void Client::loadUserConfig(QString path) {
    QFile file(path);
    if(!file.open(QFile::ReadOnly)) {
        qDebug() << "用户信息文件不存在，创建文件" << Qt::endl;
    }

    QTextStream in(&file);
    QString password, userid, userType;
    float balance;
    while(!in.atEnd()) {
        in >> userType >> userid >> password;
        if(userType.length() == 0) continue;
        if(in.status() != QTextStream::Ok) {
            throw "读取用户信息错误";
        }
        if(userType == "customer") {
            in >> balance;
            customerList.append(new CustomerUser(userid, password, balance));
        } else if (userType == "seller") {
            sellerList.append(new SellerUser(userid, password));
        } else {
            throw "未定义的用户类型";
        }
    }
}

void Client::saveUserConfig(QString path) {
    QFile file(path);

    if(!file.open(QFile::WriteOnly | QFile::Truncate)) {
        return;
    }

    QTextStream out(&file);
    for(auto index = customerList.begin(); index != customerList.end(); index++) {
        CustomerUser* customerUser = *index;
        out << "customer " << customerUser->userid << " "
            << customerUser->password << " "
            << customerUser->queryBalance() << "\n";
    }

    for(auto index = sellerList.begin(); index != sellerList.end(); index++) {
        SellerUser* sellerUser = *index;
        out << "seller " << sellerUser->userid << " "
            << sellerUser->password << "\n";
    }
    qDebug() << "保存成功" << Qt::endl;
}

void Client::saveUserConfig() {
    saveUserConfig(configFile);
}

void Client::signOutUser() {
    currentUser = nullptr;
    emit signOut();
}

void Client::openDataBase() {
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("Commodity.dat");
    database.open();
    QSqlQuery query;
    bool success;

    // 在数据库中创建新表Commodity和Event
    success = query.exec("CREATE TABLE IF NOT EXISTS Commodity(\
        Name    VARCAHR(10)     NOT NULL,\
        Description VARCHAR(50) NOT NULL,\
        Type    VARCAHR(10)     NOT NULL,\
        User    VARCHAR(10)     NOT NULL,\
        Price   FLOAT           NOT NULL,\
        Store   INT             NOT NULL\
    );");
    if(!success) throw query.lastError();

    success = query.exec("CREATE TABLE IF NOT EXISTS Event (\
        User    VARCAHR(10)     NOT NULL,\
        Type    VARCAHR(10)     NOT NULL,\
        Price   FLOAT           NOT NULL\
    );");
    if(!success) throw query.lastError();
}

void Client::changePassword(QString oldPassword, QString newPassword) {
    if(oldPassword != currentUser->getPassword())
        emit errorHappen("原密码输入错误");
    else {
        currentUser->setPassword(newPassword);
        emit infoHappen("密码修改成功");
    }
}

void Client::addNewCommoidty(QString name, QString description, QString type, QString sprice, QString sstore) {
    QSqlQuery query;
    QVector<QString> list;
    QString queryStr = "SELECT * FROM Commodity WHERE Name = '%1' AND User = '%2';";
    query.prepare(queryStr.arg(name).arg(currentUser->userid));
    if(!query.exec()) {
        qDebug() << query.lastError().driverText();
        qDebug() << query.executedQuery();
    }

    if(query.next()) {
        // commodity existing

        return;
    } else {
        queryStr = "INSERT INTO Commodity VALUES('%1', '%2', '%3', '%4', %5, %6)";
        query.prepare(queryStr.arg(name).arg(description).arg(type).arg(currentUser->getUserId()).arg(sprice).arg(sstore));
        list.append(name);
        list.append(description);
        list.append(type);
        list.append(currentUser->getUserId());
        list.append(sprice);
        list.append(sstore);
        query.exec();
    }
    BaseCommodity* element;
    QString userid = currentUser->getUserId();
    float price = sprice.toFloat();
    int store = sstore.toInt();
    if(type == "Cloth")
        element = new ClothCommodity(name, userid, description, price, store);
    if(type == "Food")
        element = new FoodCommodity(name, userid, description, price, store);
    if(type == "Book")
        element = new BookCommodity(name, userid, description, price, store);
    emit updateData(element);
#ifdef DEBUG
    qDebug() << "添加商品成功" << Qt::endl;
#endif
}

void Client::addEvent(QString type, float percent) {
    QSqlQuery query;
    QString queryStr = "SELECT * FROM Event Where User = '%1' AND Type = '%2'";
    query.exec(queryStr.arg(currentUser->getUserId()).arg(type));
    bool success;
    if(query.next()) {
        QString queryStr = "UPDATE Event SET Price=%1";
        success = query.exec(queryStr.arg(percent));
    } else {
        QString queryStr = "INSERT INTO Event VALUES('%1', '%2', %3)";
        success = query.exec(queryStr.arg(currentUser->getUserId()).arg(type).arg(percent));
    }
    if(!success) {
        emit errorHappen(query.lastError().text());
    }
    emit changeCommoditySuccess();
    emit infoHappen("折扣修改成功");
}

void Client::buyCommodity(QString name, QString user, float price, int num) {
    if(price * num > curCustomerUser->queryBalance()) {
        emit errorHappen("余额不足");
        return;
    } else {
        curCustomerUser->pay(price * num);
        emit balanceChange(curCustomerUser->queryBalance());
    }
    QSqlQuery query;
    QString queryStr = "UPDATE Commodity SET Store=Store-%1 WHERE Name = '%2' AND User = '%3'";
    query.exec(queryStr.arg(num).arg(name).arg(user));
    emit buyCommoditySuccess();
}

void Client::recharge(float n_balance) {
    curCustomerUser->recharge(n_balance);
    emit balanceChange(curCustomerUser->queryBalance());
}

void Client::changeCommodity(QString name, QString description, QString user, QString price, QString stored) {
    if(user != currentUser->getUserId()) {
        emit errorHappen("你没有修改其他商家商品的权限");
        return;
    }
    QString queryStr = "UPDATE Commodity SET Description='%1',Price=%2,Store=%3 "
            "WHERE Name = '%4' AND User = '%5'";
    QSqlQuery query;
    query.exec(queryStr.arg(description).arg(price).arg(stored).arg(name).arg(user));
    emit changeCommoditySuccess();
}

void Client::deleteCommodity(QString name, QString user) {
    if(user != currentUser->getUserId()) {
        emit errorHappen("你没有修改其他商家商品的权限");
        return;
    }
    QString queryStr = "DELETE FROM Commodity WHERE Name = '%1' AND User = '%2'";
    QSqlQuery query;
    query.exec(queryStr.arg(name).arg(user));
    emit deleteCommoditySuccess();
}

// DEBUG

int Client::debugUserCount() {
    return customerList.length() + sellerList.length();
}
