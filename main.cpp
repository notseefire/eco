#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <lib/client.h>
#include <commoditymodel.h>
#include <QObject>
#include "cartmodel.h"
#include "ordermodel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    Client* client = new Client("./233.txt");
    CommodityModel* commodityModel = new CommodityModel();
    CartModel* cartModel = new CartModel();
    OrderModel* orderModel = new OrderModel;

    client->setCart(cartModel);
    client->setOrder(orderModel);

    QObject::connect(client, &Client::updateData,
                     commodityModel, &CommodityModel::modelReset);

    QObject::connect(client, &Client::openCart,
                     cartModel, &CartModel::openCart);

    QObject::connect(client, &Client::selected,
                     cartModel, &CartModel::changeSelected);

    QObject::connect(client, &Client::dealNoLate,
                     cartModel, &CartModel::compeleteDeal);

    QObject::connect(orderModel, &OrderModel::calculateOrder,
                     client, &Client::calculateOrder);

    QQmlApplicationEngine engine;
    qmlRegisterType<Client>("com.cyks.Client", 1, 0, "Client");
    qmlRegisterType<CommodityModel>("com.cyks.CommodityModel", 1, 0, "CommodityModel");
    qmlRegisterType<CartModel>("com.cyks.CartModel", 1, 0, "CartModel");

    engine.rootContext()->setContextProperty("client", client);
    engine.rootContext()->setContextProperty("commodityModel", commodityModel);
    engine.rootContext()->setContextProperty("cartModel", cartModel);
    engine.rootContext()->setContextProperty("orderModel", orderModel);
    engine.load("qrc:/main.qml");
    return app.exec();
}
