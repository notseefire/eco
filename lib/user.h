/*
 * @Descripttion: 
 * @version: 1.0.0
 * @Author: CYKS
 * @Date: 2021-05-13 14:31:08
 * @LastEditors: CYKS
 * @LastEditTime: 2021-05-13 14:58:03
 */
#ifndef USER_H
#define USER_H
#include <QString>

enum UserType {
    Seller,
    Customer
};

/*! \class BaseUser
 *  \brief 用户基类
 */
class BaseUser {
public:
    QString userid, password;

    /*! \fn BaseUser(QString& userid, QString& password)
     *  \brief BaseUser构造函数
     *  \param userid 用户名
     *  \param password 密码
     */
    BaseUser(QString& userid, QString& password);
    void setUserId(QString& new_userid);
    void setPassword(QString&);
    QString getUserId() const;
    QString getPassword() const;
    virtual UserType getUserType() const = 0;
};

/*! \class SellerUser
 *  \brief 商家类
 */
class SellerUser: public BaseUser {
public:
    SellerUser(QString&, QString&);
    /*! \fn UserType getUserType()
     *  \brief 返回UserType::Seller，表示用户类型为商家
     */
    UserType getUserType() const override;
};

/*! \class CustomerUser
 *  \brief 消费者类
 */
class CustomerUser: public BaseUser {
public:
    CustomerUser(QString&, QString&, float = 0);
    /*! \fn UserType getUserType()
     *  \brief 返回UserType::Customer，表示用户类型为消费者
     */
    UserType getUserType() const override;
    // 用户需要提供充值和支付，查询余额等功能

    /*! \fn void recharge(float addBalance)
     *  \brief 在用户的余额内充值
     *  \param addBalance 要充值的金额大小
     */
    void recharge(float addBalance);

    /*! \fn void pay(float minusBalance)
     *  \brief 使用用户的余额进行消费
     *  \param minusBalance 消费金额的大小
     */
    void pay(float minusBalance);

    /*! \fn float queryBalance()
     *  \brief 查询用户的余额
     *  \return 返回用户余额
     */
    float queryBalance() const;
private:
    float balance;
};


#endif // USER_H
