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
        if(index.column() == 1) {
            float price = m_list.at(index.row())->getPrice();
            return price;
        }
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

Order* OrderModel::deleteOrder(int row) {
    return m_list.at(row);
}

Order* OrderModel::finishOrder(int row) {
    return m_list.at(row);
}

void OrderModel::openOrder(QList<Order*> n_list, QString cur) {
    currentUser = cur;
    beginResetModel();
    m_list = n_list;
    endResetModel();
}
