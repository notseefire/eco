#ifndef CARTMODEL_H
#define CARTMODEL_H

#include "lib/cartcommodity.h"
#include <QAbstractTableModel>
#include <QObject>
#include <QList>
#include <QVector>
#include <QSqlQuery>
#include <QDebug>
#include "../../server/command.h"


class CartModel : public QAbstractTableModel  {
public:
    CartModel(QObject *parent = nullptr);
    Q_INVOKABLE QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex & = QModelIndex()) const override;
    int columnCount(const QModelIndex & = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QHash<int, QByteArray> roleNames() const override {
        return {{Qt::DisplayRole, "display"}, {Qt::CheckStateRole, "check"}};
    }
    QJsonArray deleteRow(int row);


    int m_column;
    QList<CartCommodity *> m_list;
public slots:
    void openCart(QList<CartCommodity *> n_listm, QString cur);
    Command compeleteDeal();
    void changeSelected(int row);

private:
    QString currentUser;

};

#endif // CARTMODEL_H
