#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <lib/client.h>
#include <commoditymodel.h>
#include <QObject>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    Client* client = new Client();
    client->setConnection();
    CommodityModel* commodityModel = new CommodityModel(client);

    QObject::connect(client, &Client::updateData,
                     commodityModel, &CommodityModel::modelReset);

    QObject::connect(client, &Client::fresh,
                     commodityModel, &CommodityModel::modelFresh);

    QObject::connect(commodityModel, &CommodityModel::requestFresh,
                     client, &Client::freshData);

    client->freshData("SELECT * FROM Commodity");

    QQmlApplicationEngine engine;
    qmlRegisterType<Client>("com.cyks.Client", 1, 0, "Client");
    qmlRegisterType<Client>("com.cyks.CommodityModel", 1, 0, "CommodityModel");

    engine.rootContext()->setContextProperty("client", client);
    engine.rootContext()->setContextProperty("commodityModel", commodityModel);
    engine.load("qrc:/main.qml");
    return app.exec();
}
