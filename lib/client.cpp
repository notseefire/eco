#include "client.h"
#include <QFile>
#include <QTextStream>
#include <QDataStream>
#include <QDebug>
#include <QDir>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QSqlError>

#define DEBUG

Client::Client(QObject* parent): QObject(parent) {
    socket = new QTcpSocket(this);
    socket->connectToHost("127.0.0.1", 23333);
    in.setDevice(socket);
    in.setVersion(QDataStream::Version::Qt_5_15);
    currentUser = nullptr;
}

Client::~Client() {
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

    Command command(CommandType::SIGNIN, userid, password);
    sendMessage(command);
    QJsonObject json = waitData();
    int response = json["response"].toInt();
    if(response == UserResponseType::SignSuccess) {
        int usertype = json["usertype"].toInt();
        if(usertype == UserType::Customer) {
            curCustomerUser = new CustomerUser(json);
            currentUser = curCustomerUser;
            emit balanceChange(curCustomerUser->queryBalance());
        } else if(usertype == UserType::Seller) {
            curSellerUser = new SellerUser(json);
            currentUser = curSellerUser;
        }
        emit signIn(currentUser->getUserType() == UserType::Customer);
    }
    else if(response == UserResponseType::SignError) {
        emit errorHappen("用户名不存在或者密码错误");
    }
}

bool Client::is_login() const {
    return currentUser != nullptr;
}

QString Client::getUserId() const {
    return currentUser->getUserId();
}


bool Client::registerUser(int userType, QString userid, QString password, float balance) {
    Command command(CommandType::REGISTER, userid, password, userType);
    sendMessage(command);
    QJsonObject json = waitData();
    int response = json["response"].toInt();
    if(response == UserResponseType::Success) {
        emit infoHappen("注册成功");
    } else if(response == UserResponseType::UserExisting) {
        emit errorHappen("用户已存在");
    }
    return true;
}

void Client::signOutUser() {
    currentUser = nullptr;
    Command command(CommandType::SIGNOUT);
    sendMessage(command);
    QJsonObject json = waitData();
    if(json["response"].toInt() == UserResponseType::Error) {
        emit errorHappen("登出失败");
        return;
    }
    emit signOut();
}

void Client::changePassword(QString oldPassword, QString newPassword) {
    if(oldPassword != currentUser->getPassword())
        emit errorHappen("原密码输入错误");
    else {
        Command command(CommandType::CHANGE_PASSWORD, newPassword);
        sendMessage(command);
        QJsonObject json = waitData();
        if(json["response"] == UserResponseType::Success) {
            currentUser->setPassword(newPassword);
            emit infoHappen("密码修改成功");
        }

    }
}

void Client::addNewCommoidty(QString name, QString description, QString type, QString sprice, QString sstore) {
    QString queryStr = "SELECT * FROM Commodity WHERE Name = '%1' AND User = '%2';";
    Command command(CommandType::SELECT, queryStr.arg(name).arg(currentUser->userid));
    sendMessage(command);
    QJsonArray array = waitArray();

    if(array.begin() != array.end()) {
        // commodity existing

        return;
    } else {
        queryStr = "INSERT INTO Commodity VALUES('%1', '%2', '%3', '%4', %5, %6)";
        Command command(CommandType::UPDATE,
            queryStr.arg(name).arg(description).arg(type)
                .arg(currentUser->getUserId()).arg(sprice).arg(sstore));
        sendMessage(command);
        QJsonObject json = waitData();
        int response = json["response"].toInt();
        if(response == SqlResponse::ERROR) {
            qDebug() << json["text"].toString();
            return;
        }
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
    Command command(CommandType::QUERY_EVENT, currentUser->getUserId(), type, percent);
    sendMessage(command);
    QJsonObject json = waitData();
    if(json["response"].toInt() == SqlResponse::ERROR) {
        emit errorHappen("添加活动错误");
        return;
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

    QString queryStr = "UPDATE Commodity SET Store=Store-%1 WHERE Name = '%2' AND User = '%3'";
    Command command(CommandType::UPDATE, queryStr.arg(num).arg(name).arg(user));
    sendMessage(command);
    QJsonObject json = waitData();
    if(json["response"].toInt() == SqlResponse::ERROR) {
        emit errorHappen(json["text"].toString());
        return;
    }

    Command command2(CommandType::CHANGE_BALANCE, -price * num);
    sendMessage(command2);
    QJsonObject json2 = waitData();
    if(json2["response"].toInt() == UserResponseType::Error) {
        emit errorHappen("支付失败");
        return;
    }

    emit buyCommoditySuccess();
}

void Client::recharge(float n_balance) {
    Command command(CommandType::CHANGE_BALANCE, n_balance);
    sendMessage(command);
    QJsonObject json = waitData();
    if(json["response"].toInt() == UserResponseType::Error) {
        emit errorHappen("充值失败");
        return;
    }
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
    Command command(CommandType::UPDATE,
                    queryStr.arg(description).arg(price).arg(stored).arg(name).arg(user));
    sendMessage(command);
    QJsonObject json = waitData();
    if(json["response"].toInt() == SqlResponse::ERROR) {
        emit errorHappen(json["text"].toString());
        return;
    }
    emit changeCommoditySuccess();
}

void Client::deleteCommodity(QString name, QString user) {
    if(user != currentUser->getUserId()) {
        emit errorHappen("你没有修改其他商家商品的权限");
        return;
    }
    QString queryStr = "DELETE FROM Commodity WHERE Name = '%1' AND User = '%2'";
    Command command(CommandType::UPDATE, queryStr.arg(name).arg(user));
    sendMessage(command);
    QJsonObject json = waitData();
    if(json["response"].toInt() == SqlResponse::ERROR) {
        emit errorHappen(json["text"].toString());
        return;
    }
    emit deleteCommoditySuccess();
}

void Client::freshData(QString query) {
    Command command(CommandType::SELECT, query);
    sendMessage(command);
    QJsonArray json = waitArray();
    QList<BaseCommodity *> new_list;
    BaseCommodity* element;
    for(auto x = json.begin(); x != json.end(); x++) {
        QJsonObject jsonObject = x->toObject();
        QString type = jsonObject["type"].toString();
        if(type == "Cloth")
            element = new ClothCommodity(jsonObject);
        if(type == "Food")
            element = new FoodCommodity(jsonObject);
        if(type == "Book")
            element = new BookCommodity(jsonObject);
        new_list.append(element);
    }
    emit fresh(new_list);
}

// network

void Client::sendMessage(Command command) {
    QJsonObject json = command.toJsonObject();
    qDebug() << "send " << json;
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out << json;
    socket->write(block);
}

QJsonObject Client::waitData() {
    socket->waitForReadyRead();
    QJsonObject json;
    in >> json;
    qDebug() << "Recv " << json;
    return json;
}

QJsonArray Client::waitArray() {
    socket->waitForReadyRead();
    QJsonArray json;
    in >> json;
    qDebug() << "Recv " << json;
    return json;
}

float Client::queryCurrentBalance() {
    Command command(CommandType::QUERY_BALANCE);
    sendMessage(command);
    QJsonObject json = waitData();

    if(json["response"].toInt() == UserResponseType::Success)
        return json["balance"].toDouble();
    else
        return 0;
}

float Client::queryPercent(BaseCommodity *commodity) {
    Command command(CommandType::QUERY_PERCENT, commodity->toJsonObject());
    sendMessage(command);
    QJsonObject json = waitData();
    if(json["response"].toInt() == UserResponseType::Error) {
        emit errorHappen("查询折扣失败");
        return 1.0;
    }
    return json["percent"].toDouble();
}














