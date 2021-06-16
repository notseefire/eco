#include "ordermodel.h"
#include <QDebug>

OrderModel::OrderModel(QObject *parent) : QAbstractTableModel(parent) {
    m_list.clear();
    m_column = 2;
}

int OrderModel::columnCount(const QModelIndex &) const {
    return m_column;
}

int OrderModel::rowCount(const QModelIndex &) const {
    return m_list.length();
}

QVariant OrderModel::data(const QModelIndex &index, int role) const {
    qDebug() << index;
    switch (role) {
    case Qt::DisplayRole:
        if(index.column() == 0)
            return m_list.at(index.row())->getID();
        if(index.column() == 1)
            return m_list.at(index.row())->getPrice();
    default:
        break;
    }

    return QVariant();
}

QVariant OrderModel::headerData(int section, Qt::Orientation orientation, int role) const {
    switch(section) {
    case 0:
        return "编号";
    case 1:
        return "总金额";
    }
}

void OrderModel::deleteOrder(int row) {
    QList<CartCommodity*> list = m_list.at(row)->getList();
    for(auto element = list.begin(); element != list.end(); element++) {
        CartCommodity* commodity = *element;
        commodity->cancel();
    }
    QSqlQuery query;
    QString queryStr = "DROP TABLE OrderList%1";
    query.exec(queryStr.arg(m_list.at(row)->getID()));
    queryStr = "DELETE FROM OrderList WHERE Num=%1";
    query.exec(queryStr.arg(m_list.at(row)->getID()));
}

bool OrderModel::finishOrder(int row, float balance) {
    if(balance < m_list.at(row)->getPrice()) return false;
    QList<CartCommodity*> list = m_list.at(row)->getList();
    for(auto element = list.begin(); element != list.end(); element++) {
        CartCommodity* commodity = *element;
        float price = commodity->getNum() * commodity->getPrice();
        emit calculateOrder(commodity->getUserID(), price);
    }
    QSqlQuery query;
    QString queryStr = "DROP TABLE OrderList%1";
    query.exec(queryStr.arg(m_list.at(row)->getID()));

    queryStr = "DELETE FROM OrderList WHERE Num=%1";
    query.exec(queryStr.arg(m_list.at(row)->getID()));
    emit calculateOrder(currentUser, m_list.at(row)->getPrice());
    return true;
}

void OrderModel::openOrder(QString cur) {
    currentUser = cur;

    QSqlQuery query;
    QString queryStr = "SELECT * FROM OrderList WHERE User='%1'";
    query.exec(queryStr.arg(cur));

    beginResetModel();
    m_list.clear();
    while(query.next()) {
        int num = query.value(0).toInt();
        float price = query.value(2).toFloat();
        m_list.append(new Order(cur, num, price));
    }
    endResetModel();

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
}
