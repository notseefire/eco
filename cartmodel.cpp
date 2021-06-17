#include <QSqlError>
#include <QSqlRecord>
#include "cartmodel.h"

CartModel::CartModel(QObject *parent): QAbstractTableModel(parent)
{
    m_list.clear();
    m_column = 4;
}

void CartModel::openCart(QList<CartCommodity *> n_list, QString cur) {
    currentUser = cur;
    beginResetModel();
    m_list.clear();
    m_list = n_list;
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

QJsonArray CartModel::deleteRow(int row) {
    QJsonArray array;
    array.append(m_list.at(row)->toJsonObject());
    return array;
}

Command CartModel::compeleteDeal() {
    QJsonArray array;

    for(auto element = m_list.begin(); element != m_list.end(); element++) {
        array.append((*element)->toJsonObject());
    }

    return Command(CommandType::CART_DEAL, array);
}

int CartModel::columnCount(const QModelIndex &) const {
    return m_column;
}
