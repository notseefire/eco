#include "commoditymodel.h"
#include <QSqlQuery>
#include <QSqlRecord>

CommodityModel::CommodityModel(Client* n_client, QObject* parent): QAbstractTableModel(parent) {
    client = n_client;
    connect(this, &CommodityModel::onModelReset,
            this, &CommodityModel::modelReset);

    /* Commodity Data Table
        Name    VARCAHR(10)     NOT NULL
        Description VARCHAR(50) NOT NULL
        Type    VARCAHR(10)     NOT NULL
        User    VARCHAR(10)     NOT NULL
        Price   INT             NOT NULL
        Store   INT             NOT NULL
    */
    m_column = 6;


    conditionMask = (1 << 3) - 1;
}

int CommodityModel::rowCount(const QModelIndex &) const {
    return m_list.length();
}

int CommodityModel::columnCount(const QModelIndex &) const {
    return m_column;
}

QVariant CommodityModel::headerData(int section, Qt::Orientation orientation, int role) const {
    switch(section) {
    case 0:
        return "名称";
    case 1:
        return "商品描述";
    case 2:
        return "类型";
    case 3:
        return "商家";
    case 4:
        return "价格";
    case 5:
        return "库存";
    }
}

QVariant CommodityModel::data(const QModelIndex &index, int role) const {
    switch (role) {
    case Qt::DisplayRole:
        if(index.column() == 0)
            return m_list.at(index.row())->getName();
        if(index.column() == 1)
            return m_list.at(index.row())->getDescription();
        if(index.column() == 2)
            return m_list.at(index.row())->getType();
        if(index.column() == 3)
            return m_list.at(index.row())->getUserID();
        if(index.column() == 4)
            return QString("%1").arg(m_list.at(index.row())->getPrice());
        if(index.column() == 5)
            return QString("%1").arg(m_list.at(index.row())->getStore());
    default:
        break;
    }

    return QVariant();
}

void CommodityModel::search(QString searchText) {
    QSqlQuery query;
    QString queryStr = "SELECT * FROM Commodity";
    QString nameStr;
    QString conditionStr;
    if(!(conditionMask & CommodityType::Book)) {
        if(conditionStr.length() != 0) conditionStr.append(" AND ");
        conditionStr.append("Type != 'Book'");
    }
    if(!(conditionMask & CommodityType::Food)) {
        if(conditionStr.length() != 0) conditionStr.append(" AND ");
        conditionStr.append("Type != 'Food'");
    }
    if(!(conditionMask & CommodityType::Cloth)) {
        if(conditionStr.length() != 0) conditionStr.append(" AND ");
        conditionStr.append("Type != 'Cloth'");
    }

    if(searchText.length()) {
        nameStr.append("Name Like '");
        for(auto index = searchText.begin(); index != searchText.end(); index++) {
            nameStr.append(*index);
            nameStr.append("%");
        }
        nameStr.append("'");
    }

    if(!nameStr.isEmpty() || !conditionStr.isEmpty()) {
        bool initialCase = true;
        queryStr.append(" WHERE ");
        if(!nameStr.isEmpty()) {
            initialCase = false;
            queryStr.append(nameStr);
        }

        if(!conditionStr.isEmpty()) {
            if(initialCase) initialCase = false;
            else queryStr.append(" AND ");
            queryStr.append(conditionStr);
        }
    }

    emit requestFresh(queryStr);

    /*
    query.exec(queryStr);
    beginResetModel();
    for(auto index = m_list.begin(); index != m_list.end(); index++)
        delete (*index);
    m_list.clear();
    while(query.next()) {
        BaseCommodity* element;
        QString name = query.value(0).toString();
        QString description = query.value(1).toString();
        QString type = query.value(2).toString();
        QString userid = query.value(3).toString();
        float price = query.value(4).toString().toFloat();
        int store = query.value(5).toString().toInt();
        if(type == "Cloth")
            element = new ClothCommodity(name, userid, description, price, store);
        if(type == "Food")
            element = new FoodCommodity(name, userid, description, price, store);
        if(type == "Book")
            element = new BookCommodity(name, userid, description, price, store);
        m_list.append(element);
    }
    endResetModel();
    */
}

void CommodityModel::insertCondition(int choice) {
    conditionMask |= (1 << choice);
}

void CommodityModel::deleteCondition(int choice) {
    if(conditionMask & (1 << choice))
        conditionMask -= (1 << choice);
    else {
        qDebug() << choice << "has been deleted before";
    }
}


int CommodityModel::getNumberData(int row, int col) {
    if(col == 5)
        return m_list.at(row)->getStore();

}

QString CommodityModel::getStringData(int row, int col) {
    if(col == 0)
        return m_list.at(row)->getName();
    if(col == 1)
        return m_list.at(row)->getDescription();
    if(col == 2)
        return m_list.at(row)->getType();
    if(col == 3)
        return m_list.at(row)->getUserID();
}

double CommodityModel::getDoubleData(int row, int col) {
    if(col == 4) {
        BaseCommodity* commodity = m_list.at(row);
        return commodity->getPrice();
    }
    if(col == 6) {
        BaseCommodity* commodity = m_list.at(row);
        double percent = client->queryPercent(commodity);
        int int_percent = percent * 100.0;
        percent = int_percent / 100.0;
        return percent;
    }
}
