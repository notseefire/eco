#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <lib/client.h>
#include <commoditymodel.h>
#include <QObject>
#include "cartmodel.h"
#include "ordermodel.h"
#include "cartmodel.h"
#include "ordermodel.h"


int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    Client* client = new Client();
    client->setConnection();
    CommodityModel* commodityModel = new CommodityModel(client);
    CartModel* cartModel = new CartModel();
    OrderModel* orderModel = new OrderModel();

    client->setCart(cartModel);
    client->setOrder(orderModel);

    QObject::connect(client, &Client::updateData,
                     commodityModel, &CommodityModel::modelReset);

    QObject::connect(client, &Client::fresh,
                     commodityModel, &CommodityModel::modelFresh);

    QObject::connect(commodityModel, &CommodityModel::requestFresh,
                     client, &Client::freshData);

    QObject::connect(client, &Client::selected,
                         cartModel, &CartModel::changeSelected);

    QObject::connect(client, &Client::dealNoLate,
                         cartModel, &CartModel::compeleteDeal);

    QObject::connect(orderModel, &OrderModel::calculateOrder,
                         client, &Client::calculateOrder);

    client->freshData("SELECT * FROM Commodity");

    QQmlApplicationEngine engine;
    qmlRegisterType<Client>("com.cyks.Client", 1, 0, "Client");
    qmlRegisterType<Client>("com.cyks.CommodityModel", 1, 0, "CommodityModel");

    engine.rootContext()->setContextProperty("client", client);
    engine.rootContext()->setContextProperty("commodityModel", commodityModel);
    engine.rootContext()->setContextProperty("cartModel", cartModel);
    engine.rootContext()->setContextProperty("orderModel", orderModel);
    engine.load("qrc:/main.qml");
    return app.exec();
}
