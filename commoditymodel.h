#ifndef COMMODITYMODEL_H
#define COMMODITYMODEL_H

#include "lib/commodity.h"
#include <QAbstractTableModel>
#include <QObject>
#include <QList>
#include <QVector>
#include <QSqlQuery>
#include <QDebug>

class CommodityModel : public QAbstractTableModel
{
Q_OBJECT

public:
    // 0 for Food
    // 1 for Cloth
    // 2 for Book
    enum CommodityType {
        Food = 1 << 0,
        Cloth = 1 << 1,
        Book = 1 << 2,
    };

    explicit CommodityModel(QObject *parent = nullptr);
    int rowCount(const QModelIndex & = QModelIndex()) const override;
    int columnCount(const QModelIndex & = QModelIndex()) const override;
    Q_INVOKABLE QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE void search(QString searchText);
    Q_INVOKABLE void insertCondition(int choice);
    Q_INVOKABLE void deleteCondition(int choice);
    Q_INVOKABLE int  getNumberData(int row, int col);
    Q_INVOKABLE QString  getStringData(int row, int col);
    Q_INVOKABLE double  getDoubleData(int row, int col);
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override {
        return {{Qt::DisplayRole, "display"}};
    }


signals:
    void onModelReset(BaseCommodity* list);

public slots:
    void modelReset(BaseCommodity* list) {
        beginResetModel();
        m_list.append(list);
        endResetModel();
    }

private:
    QList<BaseCommodity*> m_list;
    int m_column;
    unsigned int conditionMask;
};

#endif // COMMODITYMODEL_H
