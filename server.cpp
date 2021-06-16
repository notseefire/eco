#include "server.h"

#include <iostream>
#include <QDir>
#include <QSqlError>

using namespace std;


Server::Server(QWidget *parent) : QWidget(parent)
{
    openDataBase();
    QDir qdir = QDir::home();
    qdir.mkdir("eco");
    loadUserConfig(QDir::homePath() + "/eco/config.txt");
    nthread = 0;
    server = new QTcpServer(this);
    server->listen(QHostAddress::LocalHost, 23333);
    connect(server, &QTcpServer::newConnection, this, &Server::newConnection);
}

Server::~Server() {
    qDebug() << "程序退出";
    saveUserConfig(QDir::homePath() + "/eco/config.txt");
}

void Server::newConnection() {
    QDataStream in;
    QTcpSocket* newSocket = server->nextPendingConnection();
    Thread *thread = new Thread(nthread++, newSocket);
    connect(thread, &Thread::execQuery, this, &Server::sqlCommandExec);
    connect(thread, &Thread::execUser, this, &Server::userCommandExec);
    connect(thread, &Thread::execEvent, this, &Server::eventCommandExec);
    connect(thread, &Thread::closeThread, this, &Server::closeThread);
    threadPool.push_back(thread);
}

void Server::closeThread(Thread* thread) {
    for(auto index = threadPool.begin(); index != threadPool.end(); index++) {
        Thread* curThread = *index;
        if(curThread->get_id() == thread->get_id()) {
            threadPool.erase(index);
            break;
        }
    }
}

Thread* Server::find_socket(unsigned char p_id) {
    for (auto index = threadPool.begin(); index != threadPool.end(); index++) {
        Thread* cur = *index;
        if(cur->get_id() == p_id) return cur;
    }
    return nullptr;
}

void Server::aboutToQuit() {
    qDebug() << "程序退出";
    saveUserConfig(QDir::homePath() + "/eco/config.txt");
}

void Server::loadUserConfig(QString path) {
    QFile file(path);
    qDebug() << path;
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

void Server::saveUserConfig(QString path) {
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
    qDebug() << "保存成功";
}

void Server::openDataBase() {
    database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("Commodity.dat");
    database.open();
    QSqlQuery query;
    bool success;

    try {
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
    }  catch (QSqlError err) {
        qDebug() << err.text();
        exit(0);
    }
}

void Server::sqlCommandExec(unsigned char p_id, Command command) {
    QSqlQuery query(database);
    QJsonArray m_list;
    Thread* thread = find_socket(p_id);
    bool success = query.exec(command.getQuery());
    if(command.getType() == CommandType::UPDATE) {
        QJsonObject json;
        if(success) {
            json["response"] = SqlResponse::SUCCESS;
        } else {
            json["response"] = SqlResponse::ERROR;
            json["text"] = query.lastError().text();
        }
        thread->sendInfoMessage(json);
    }
    if(command.getType() == CommandType::SELECT) {
        while(query.next()) {
            QJsonObject json;
            json["name"] = query.value(0).toString();
            json["description"] = query.value(1).toString();
            json["type"] = query.value(2).toString();
            json["userid"] = query.value(3).toString();
            json["price"] = query.value(4).toString().toFloat();
            json["store"] = query.value(5).toString().toInt();
            m_list.append(json);
        }
        thread->sendMessage(m_list);
    }
}

// User Manager

UserResult Server::loginUser(QString userid, QString password) {
    for(auto index = customerList.begin(); index != customerList.end(); index++) {
        BaseUser* baseUser = *index;
        if (baseUser->userid == userid) {
            if (baseUser->password == password) {
                return UserResult {
                    baseUser->toJsonObject(),
                    UserResponseType::SignSuccess,
                    baseUser,
                };
            }
        }
    }

    for(auto index = sellerList.begin(); index != sellerList.end(); index++) {
        BaseUser* baseUser = *index;
        if (baseUser->userid == userid) {
            if (baseUser->password == password) {
                return UserResult {
                    baseUser->toJsonObject(),
                    UserResponseType::SignSuccess,
                    baseUser,
                };
            }
        }
    }
    return UserResult {
        QJsonObject(),
        UserResponseType::SignError,
        nullptr,
    };
}

UserResponseType Server::registerUser(int userType, QString userid, QString password) {
    for(auto index = customerList.begin(); index != customerList.end(); index++) {
        BaseUser* baseUser = *index;
        if (baseUser->userid == userid) {
            return UserResponseType::UserExisting;
        }
    }

    for(auto index = sellerList.begin(); index != sellerList.end(); index++) {
        BaseUser* baseUser = *index;
        if (baseUser->userid == userid) {
            qDebug() << "Register Error" << Qt::endl;
            return UserResponseType::UserExisting;
        }
    }

    // 0 for Customer, 1 for Seller
    switch (userType) {
        case 0:
            customerList.append(new CustomerUser(userid, password, 0));
        break;
        case 1:
            sellerList.append(new SellerUser(userid, password));
        break;
    }
    return UserResponseType::Success;
}

void Server::userCommandExec(unsigned char p_id, Command command) {
    Thread *socket = find_socket(p_id);
    QJsonObject json;
    if(command.getType() == CommandType::REGISTER) {
        UserResponseType type = registerUser(command.usertype, command.userid, command.password);
        switch (type) {
        case UserResponseType::UserExisting:
            json["response"] = UserResponseType::UserExisting;
        break;
        case UserResponseType::Success:
            json["response"] = UserResponseType::Success;
        break;
        }
        socket->sendInfoMessage(json);
    }
    if(command.getType() == CommandType::SIGNIN) {
        UserResult result = loginUser(command.userid, command.password);
        json = result.userinfo;
        switch (result.type) {
        case UserResponseType::SignError:
            json["response"] = UserResponseType::SignError;
            break;
        case UserResponseType::SignSuccess:
            socket->login(result.user);
            json["response"] = UserResponseType::SignSuccess;
            break;
        }
        socket->sendInfoMessage(json);
    }
    if(command.getType() == CommandType::CHANGE_PASSWORD) {
        BaseUser* user = socket->getCurrentUser();
        if(user == nullptr) {
            json["response"] = UserResponseType::Error;
        } else {
            QString newPassword = command.getQuery();
            json["response"] = UserResponseType::Success;
            user->setPassword(newPassword);
        }
        socket->sendInfoMessage(json);
    }
    if(command.getType() == CommandType::QUERY_BALANCE) {
        BaseUser* user = socket->getCurrentUser();
        if(user == nullptr || user->getUserType() != UserType::Customer) {
            json["response"] = UserResponseType::Error;
        } else {
            json["response"] = UserResponseType::Success;
            for(auto index = customerList.begin(); index != customerList.end(); index++) {
                BaseUser* baseUser = *index;
                if (baseUser->userid == user->userid) {
                    json["balance"] = (*index)->queryBalance();
                }
            }
        }
        socket->sendInfoMessage(json);
    }
    if(command.getType() == CommandType::CHANGE_BALANCE) {
        BaseUser* user = socket->getCurrentUser();
        float balance = command.balance;
        if(user == nullptr || user->getUserType() != UserType::Customer) {
            json["response"] = UserResponseType::Error;
        } else {
            json["response"] = UserResponseType::Success;
            for(auto index = customerList.begin(); index != customerList.end(); index++) {
                BaseUser* baseUser = *index;
                if (baseUser->userid == user->userid) {
                    if(balance < 0)
                        (*index)->pay(-balance);
                    else
                        (*index)->recharge(balance);
                }
            }
        }
        socket->sendInfoMessage(json);
    }
    if(command.getType() == CommandType::SIGNOUT) {
        BaseUser* user = socket->getCurrentUser();
        if(user == nullptr)
            json["response"] = UserResponseType::Error;
        else {
            json["response"] = UserResponseType::Success;
            socket->signout();
        }
        socket->sendInfoMessage(json);
    }
    if(command.getType() == CommandType::QUERY_PERCENT) {
        QJsonObject commodity_json = command.commodity;
        QString type = commodity_json["type"].toString();
        BaseCommodity* commodity;
        if(type == "Food") {
            commodity = new FoodCommodity(commodity_json);
        } else if(type == "Cloth") {
            commodity = new ClothCommodity(commodity_json);
        } else if(type == "Book") {
            commodity = new BookCommodity(commodity_json);
        }
        json["response"] = UserResponseType::Success;
        json["percent"] = commodity->getActualPrice();
        socket->sendInfoMessage(json);
    }
}

// Event Manager

void Server::eventCommandExec(unsigned char p_id, Command command) {
    Thread* socket = find_socket(p_id);
    QJsonObject json;
    QSqlQuery query;
    qDebug() << command.userid << " " << command.commodity_type;
    QString queryStr = "SELECT * FROM Event Where User = '%1' AND Type = '%2'";
    query.exec(queryStr.arg(command.userid).arg(command.commodity_type));
    bool success;
    if(query.next()) {
        QString queryStr = "UPDATE Event SET Price=%1";
        success = query.exec(queryStr.arg(command.percent));
        qDebug() << queryStr.arg(command.percent);
    } else {
        QString queryStr = "INSERT INTO Event VALUES('%1', '%2', %3)";
        success = query.exec(queryStr.arg(command.userid)
                             .arg(command.commodity_type).arg(command.percent));
        qDebug() << queryStr.arg(command.userid)
                    .arg(command.commodity_type).arg(command.percent);
    }
    if(success) {
        json["response"] = SqlResponse::SUCCESS;
    } else {
        json["response"] = SqlResponse::ERROR;
    }
    socket->sendInfoMessage(json);
}
