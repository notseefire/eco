#include "server.h"

#include <iostream>
#include <QDir>
#include <QSqlError>
#include <QHBoxLayout>
#include <QLabel>
#include "../../eco/eco/lib/cartcommodity.h"
#include "../../eco/eco/lib/order.h"

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
    setUI();
}

void Server::setUI() {
    QHBoxLayout* layout = new QHBoxLayout();
    setLayout(layout);
    QLabel* name = new QLabel("ECO Server");
    QLabel* label_ip = new QLabel("Listening on port 23333");
    layout->addWidget(name);
    layout->addWidget(label_ip);
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
    connect(thread, &Thread::execCart, this, &Server::cartCommandExec);
    connect(thread, &Thread::execOrder, this, &Server::orderCommandExec);
    connect(thread, &Thread::closeThread, this, &Server::closeThread);
    threadPool.push_back(thread);
}

void Server::calculateOrder(QString userid, float money) {
    qDebug() << userid << " " << money;
    BaseUser* baseUser;
    for(auto index = customerList.begin(); index != customerList.end(); index++) {
        baseUser = *index;
        if (baseUser->userid == userid) {
            (*index)->pay(money);
            return;
        }
    }

    for(auto index = sellerList.begin(); index != sellerList.end(); index++) {
        baseUser = *index;
        if (baseUser->userid == userid) {
            (*index)->addMoney(money);
            return;
        }
    }
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

    QSqlQuery query;
        QString queryStr = "CREATE TABLE IF NOT EXISTS %1_Cart(\
                    Name    VARCAHR(10)     NOT NULL,\
                    User    VARCHAR(10)     NOT NULL,\
                    Num   INT             NOT NULL\
                );";

    QTextStream in(&file);
    QString password, userid, userType;
    float balance;
    while(!in.atEnd()) {
        in >> userType >> userid >> password >> balance;
        if(userType.length() == 0) continue;
        if(in.status() != QTextStream::Ok) {
            throw "读取用户信息错误";
        }
        if(userType == "customer") {
            bool success = query.exec(queryStr.arg(userid));
            if(!success) throw query.lastError();
            customerList.append(new CustomerUser(userid, password, balance));
        } else if (userType == "seller") {
            sellerList.append(new SellerUser(userid, password, balance));
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
            << sellerUser->password << " " << sellerUser->getMoney() << "\n";
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

        success = query.exec("CREATE TABLE IF NOT EXISTS OrderList (\
            Num     INT             NOT NULL,\
            User    VARCAHR(10)     NOT NULL,\
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
        case 0: {
            customerList.append(new CustomerUser(userid, password, 0));
            QSqlQuery query;
            QString queryStr = "CREATE TABLE IF NOT EXISTS %1_Cart(\
                            Name    VARCAHR(10)     NOT NULL,\
                            User    VARCHAR(10)     NOT NULL,\
                            Num   INT             NOT NULL\
                        );";
            query.exec(queryStr.arg(userid));
            break;
        }
        case 1:
            sellerList.append(new SellerUser(userid, password, 0));
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
                    qDebug() << (*index)->queryBalance();
                    json["balance"] = (*index)->queryBalance();
                }
            }
        }
        socket->sendInfoMessage(json);
    }
    if(command.getType() == CommandType::CHANGE_BALANCE) {
        BaseUser* user = socket->getCurrentUser();
        float balance = command.balance;
        if(user == nullptr) {
            json["response"] = UserResponseType::Error;
        } else {
            json["response"] = UserResponseType::Success;
            for(auto index = sellerList.begin(); index != sellerList.end(); index++) {
                BaseUser* baseUser = *index;
                if (baseUser->userid == user->userid) {
                    (*index)->addMoney(balance);
                }
            }

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
        float percent = commodity->getActualPrice();
        int ipercent = percent * 100.0;
        percent = ipercent / 100.0;
        json["percent"] = percent;
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

void Server::cartCommandExec(unsigned char p_id, Command command) {
    QSqlQuery query;
    Thread* socket = find_socket(p_id);
    if(command.getType() == CommandType::CART_UPDATE) {
        QJsonObject json;
        json["response"] = SqlResponse::SUCCESS;
        QString cur = command.cur;
        QString userid = command.userid;
        QString name = command.name;
        int num = command.num;
        QString queryStr = "SELECT * FROM %1_Cart WHERE User='%2' AND Name='%3'";
        query.exec(queryStr.arg(cur).arg(userid).arg(name));
        if(query.next()) {
            queryStr = "UPDATE %1_Cart SET Num=Num+%2 "
                "WHERE Name='%3' AND User='%4'";
            query.exec(queryStr.arg(cur).arg(num).arg(name).arg(userid));
        } else {
            queryStr = "INSERT INTO %1_Cart VALUES('%2', '%3', %4)";
            query.exec(queryStr.arg(cur).arg(name).arg(userid).arg(num));
            qDebug() << queryStr.arg(cur).arg(name).arg(userid).arg(num);
        }
        socket->sendInfoMessage(json);
    } else if(command.getType() == CommandType::CART_SELECT) {
        QJsonArray array;
        QString queryStr = command.getQuery();
        query.exec(queryStr);
        /* Commodity Data Table
            Name    VARCAHR(10)     NOT NULL
            Description VARCHAR(50) NOT NULL
            Type    VARCAHR(10)     NOT NULL
            User    VARCHAR(10)     NOT NULL
            Price   INT             NOT NULL
            Store   INT             NOT NULL
        */
        while(query.next()) {
            QString name = query.value(0).toString();
            QString userid = query.value(1).toString();
            int store = query.value(2).toString().toInt();
            CartCommodity* commodity =
                    new CartCommodity(socket->getCurrentUser()->getUserId(), name, userid, store);
            array.append(commodity->toJsonObject());
        }
        socket->sendMessage(array);
    } else if(command.getType() == CommandType::CART_DEAL) {
        QJsonArray array = command.list;
        QJsonObject json;
        QList<CartCommodity*> m_list;
        float price = 0;
        int max_id = 0;
        QString currentUser = socket->getCurrentUser()->getUserId();

        for(auto index = array.begin(); index != array.end(); index++) {
            m_list.append(new CartCommodity((*index).toObject()));
        }

        for(auto element = m_list.begin(); element != m_list.end(); element++) {
            CartCommodity* commodity = *element;
            if(commodity->getSelected()) {
                int store = commodity->getStore();
                if(store < commodity->getNum()) {
                    json["response"] = SqlResponse::ERROR;
                    json["text"] = commodity->getName();
                }
                price += commodity->getPrice() * commodity->getNum();
            }
        }
        if(json.contains("response")) {
            socket->sendInfoMessage(json);
            return;
        }
        QSqlQuery query;
        QString queryStr = "SELECT MAX(Num) FROM OrderList";
        query.exec(queryStr);
        if(query.next()) {
            max_id = query.value(0).toInt() + 1;
        }
        queryStr = "INSERT INTO OrderList VALUES(%1, '%2', %3)";
        query.exec(queryStr.arg(max_id).arg(currentUser).arg(price));
        qDebug() << queryStr.arg(max_id).arg(currentUser).arg(price);

        queryStr = "CREATE TABLE IF NOT EXISTS OrderList%1(\
                    Name    VARCAHR(10)     NOT NULL,\
                    User    VARCHAR(10)     NOT NULL,\
                    Num   INT             NOT NULL\
                );";
        query.exec(queryStr.arg(max_id));
        queryStr = "INSERT INTO OrderList%1 VALUES ('%2', '%3', %4)";

        for(auto element = m_list.begin(); element != m_list.end(); element++) {
            CartCommodity* commodity = *element;
            if(commodity->getSelected()) {
                query.exec(queryStr.arg(max_id).arg(commodity->getName())
                                                    .arg(commodity->getUserID()).arg(commodity->getNum()));
                commodity->deleteFromCart();
                commodity->finishFromCart();
            }
        }
        json["response"] = SqlResponse::SUCCESS;
        socket->sendInfoMessage(json);
    } else if(command.getType() == CommandType::CART_DELETE) {
        QJsonArray array = command.list;
        QJsonObject json;
        json["response"] = SqlResponse::SUCCESS;
        CartCommodity* commodity = new CartCommodity((*array.begin()).toObject());
        commodity->deleteFromCart();
        socket->sendInfoMessage(json);
    }
}

void Server::orderCommandExec(unsigned char p_id, Command command) {
    Thread* socket = find_socket(p_id);
    QSqlQuery query;
    if(command.getType() == CommandType::ORDER_SELECT) {
        QJsonArray array;
        QString cur = command.getQuery();
        QSqlQuery query;
        QString queryStr = "SELECT * FROM OrderList WHERE User='%1'";
        query.exec(queryStr.arg(cur));
        QList<Order*> m_list;

        while(query.next()) {
            int num = query.value(0).toInt();
            float price = query.value(2).toFloat();
            m_list.append(new Order(cur, num, price));
        }

        queryStr = "SELECT * FROM OrderList%1";
        for(auto element = m_list.begin(); element != m_list.end(); element++) {
            Order* order = *element;
            query.exec(queryStr.arg(order->getID()));
            while(query.next()) {
                QString name = query.value(0).toString();
                QString userid = query.value(1).toString();
                int num = query.value(2).toInt();
                order->addCommodity(name, userid, num);
            }
        }

        for(auto element = m_list.begin(); element != m_list.end(); element++) {
            array.append((*element)->toJsonObject());
        }

        socket->sendMessage(array);
    } else if(command.getType() == ORDER_DELETE) {
        QJsonObject json;
        json["response"] = SqlResponse::SUCCESS;
        Order* order = new Order(command.commodity);
        QList<CartCommodity*> list = order->getList();
        for(auto element = list.begin(); element != list.end(); element++) {
            CartCommodity* commodity = *element;
            commodity->cancel();
        }
        QSqlQuery query;
        QString queryStr = "DROP TABLE OrderList%1";
        query.exec(queryStr.arg(order->getID()));
        queryStr = "DELETE FROM OrderList WHERE Num=%1";
        query.exec(queryStr.arg(order->getID()));
        socket->sendInfoMessage(json);
    } else if(command.getType() == ORDER_FINISH) {
        QJsonObject json;
        Order* order = new Order(command.commodity);
        QList<CartCommodity*> list = order->getList();
        for(auto element = list.begin(); element != list.end(); element++) {
            CartCommodity* commodity = *element;
            float price = commodity->getNum() * commodity->getPrice();
            calculateOrder(commodity->getUserID(), price);
        }
        QSqlQuery query;
        QString queryStr = "DROP TABLE OrderList%1";
        query.exec(queryStr.arg(order->getID()));

        queryStr = "DELETE FROM OrderList WHERE Num=%1";
        query.exec(queryStr.arg(order->getID()));
        calculateOrder(socket->getCurrentUser()->getUserId(), order->getPrice());
        json["response"] = SqlResponse::SUCCESS;
        socket->sendInfoMessage(json);
    }
}
