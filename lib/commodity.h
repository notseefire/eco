#ifndef COMMODITY_H
#define COMMODITY_H
#include <QString>
#include <QJsonArray>
#include <QJsonObject>

/*! \class BaseCommodity
 *  \brief 商品基类
 */
class BaseCommodity
{
public:
    BaseCommodity(QString name, QString userid, QString description, float price, int store);
    BaseCommodity(QJsonObject& json);
    virtual ~BaseCommodity();
    virtual float getActualPrice() const = 0 ;
    virtual QString getType() const = 0;
    QString getName() const;
    float   getPrice() const;
    void    setPrice(float n_price);
    QString getUserID() const;
    QString getDescription() const;
    void    setDescription(QString n_description);
    int     getStore() const;
    void    setStore(int n_store);
    QJsonObject toJsonObject();


private:
    QString name, userid, description;
    float price;
    int store;
};

/*! \class ClothCommodity
 *  \brief 服装类商品
 */
class ClothCommodity : public BaseCommodity
{
public:
    ClothCommodity(QString name, QString userid, QString description, float price, int store);
    ClothCommodity(QJsonObject& json);
    float getActualPrice() const override;
    QString getType() const override;
};

/*! \class FoodCommodity
 *  \brief 食品类商品
 */
class FoodCommodity : public BaseCommodity
{
public:
    FoodCommodity(QString name, QString userid, QString description, float price, int store);
    FoodCommodity(QJsonObject& json);
    float getActualPrice() const override;
    QString getType() const override;
};

/*! \class BookCommodity
 *  \brief 书类商品
 */
class BookCommodity : public BaseCommodity
{
public:
    BookCommodity(QString name, QString userid, QString description, float price, int store);
    BookCommodity(QJsonObject& json);
    float getActualPrice() const override;
    QString getType() const override;
};

#endif // COMMODITY_H
