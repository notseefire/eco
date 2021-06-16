#ifndef ORDERMODEL_H
#define ORDERMODEL_H

#include <QAbstractTableModel>
#include <QList>
#include "lib/order.h"
#include <QObject>

class OrderModel : public QAbstractTableModel{
    Q_OBJECT
public:
    explicit OrderModel(QObject *parent = nullptr);
    Q_INVOKABLE QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex & = QModelIndex()) const override;
    int columnCount(const QModelIndex & = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override {
        return {{Qt::DisplayRole, "display"}};
    }

    void openOrder(QString cur);
    void deleteOrder(int row);
    bool finishOrder(int row, float balance);

signals:
    void calculateOrder(QString userid, float money);

private:
    QString currentUser;
    QList<Order*> m_list;
    int m_column;
};

#endif // ORDERMODEL_H
