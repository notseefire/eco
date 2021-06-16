#include <QSqlError>
#include <QSqlRecord>
#include "cartmodel.h"

CartModel::CartModel(QObject *parent): QAbstractTableModel(parent)
{
    m_list.clear();
    m_column = 4;
}

void CartModel::openCart(QString cur) {
    currentUser = cur;
    m_list.clear();
    QSqlQuery query, query_commodity;
    QString queryStr = "SELECT * FROM %1_Cart";
    query.exec(queryStr.arg(cur));
    /* Commodity Data Table
        Name    VARCAHR(10)     NOT NULL
        Description VARCHAR(50) NOT NULL
        Type    VARCAHR(10)     NOT NULL
        User    VARCHAR(10)     NOT NULL
        Price   INT             NOT NULL
        Store   INT             NOT NULL
    */
    beginResetModel();
    while(query.next()) {
        QString name = query.value(0).toString();
        QString userid = query.value(1).toString();
        int store = query.value(2).toString().toInt();
        m_list.append(new CartCommodity(cur, name, userid, store));
    }
    endResetModel();
}

QVariant CartModel::data(const QModelIndex &index, int role) const {
    switch (role) {
    case Qt::DisplayRole:
        if(index.column() == 0)
            return m_list.at(index.row())->getName();
        if(index.column() == 1)
            return m_list.at(index.row())->getUserID();
        if(index.column() == 2)
            return QString("%1").arg(m_list.at(index.row())->getNum());
    case Qt::CheckStateRole:
        if(m_list.at(index.row())->getSelected())
            return Qt::Checked;
        else
            return Qt::Unchecked;
    default:
        break;
    }

    return QVariant();
}

void CartModel::changeSelected(int row) {
    m_list.at(row)->changeSelect();
}

QVariant CartModel::headerData(int section, Qt::Orientation orientation, int role) const {
    switch(section) {
    case 0:
        return "名称";
    case 1:
        return "商家";
    case 2:
        return "数量";
    case 3:
        return "购买";
    }
}

int CartModel::rowCount(const QModelIndex &) const {
    return m_list.length();
}

void CartModel::deleteRow(int row) {
    m_list.at(row)->deleteFromCart();
}

QString CartModel::compeleteDeal() {
    int max_id = 0;
    float price = 0.0;
    for(auto element = m_list.begin(); element != m_list.end(); element++) {
        CartCommodity* commodity = *element;
        if(commodity->getSelected()) {
            int store = commodity->getStore();
            if(store < commodity->getNum()) return commodity->getName();
            price += commodity->getPrice() * commodity->getNum();
        }
    }
    QSqlQuery query;
    QString queryStr = "SELECT MAX(Num) FROM OrderList";
    query.exec(queryStr);
    if(query.next()) {
        max_id = query.value(0).toInt() + 1;
    }
    queryStr = "INSERT INTO OrderList VALUES(%1, '%2', %3)";
    query.exec(queryStr.arg(max_id).arg(currentUser).arg(price));

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
    return "";
}

int CartModel::columnCount(const QModelIndex &) const {
    return m_column;
}

