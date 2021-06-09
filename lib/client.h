#ifndef CLIENT_H
#define CLIENT_H

#include <lib/commodity.h>
#include <lib/user.h>
#include <QList>
#include <QFile>
#include <QSqlDatabase>
#include <QObject>
#include <QRegExp>


/*! \class Client
 *  \brief 客户端类
 *
 *  客户端目前的设计是前后端交互的窗口，在后面将会拓展成C/S架构
 */
class Client : public QObject {
    Q_OBJECT

public:
    Client(QString = "qrc:/config.txt", QObject *parent = 0);
    ~Client() override;

    /*! \fn void loadUserConfig(QString path)
     *  \brief 载入用户信息
     *  \param  用户信息的所在路径
     */
    void loadUserConfig(QString path);

    /*! \fn void saveUserConfig(QString path)
     *  \brief 保存用户信息
     *  \param  用户信息的所在路径
     */
    void saveUserConfig(QString path);

    /*! \fn void openDataBase()
     *  \brief 打开数据库文件
     */
    void openDataBase();

    /*! \fn int debugUserCount()
     *  \brief  获取用户数量
     *  \return 目前用户数量
     */
    int debugUserCount();

    /*! \fn void addNewCommoidty(QString name, QString description, QString type, QString sprice, QString sstore)
     *  \brief 向数据库中添加 新的商品
     *  \param name 商品名
     *  \param description 商品描述
     *  \param type 商品类
     *  \param sprice 商品价格
     *  \param sstore 商品库存
     */
    Q_INVOKABLE void addNewCommoidty(QString name, QString description, QString type, QString sprice, QString sstore);

    /*! \fn void loginUser(QString userid, QString password)
     *  \brief 登录用户，登陆成功后将会设置客户端用户
     *  \param userid 用户名
     *  \param password 密码
     */
    Q_INVOKABLE void loginUser(QString userid, QString password);

    /*! \fn void signOutUser()
     *  \brief 注销登录，成功后将会设置客户端用户为空
     */
    Q_INVOKABLE void signOutUser();

    /*! \fn void saveUserConfig()
     *  \brief 提供给界面类保存用户信息的接口
     */
    Q_INVOKABLE void saveUserConfig();

    /*! \fn bool is_login()
     *  \brief 查询当前客户端是否有用户登录
     *  \return 返回 true 表示已经登录，否则为未登录
     */
    Q_INVOKABLE bool is_login() const;

    /*! \fn void changePassword(QString oldPassword, QString newPassword)
     *  \brief 修改用户密码，需要提供旧密码作为验证，将密码修改为新密码
     *  \param oldPassword 旧密码
     *  \param newPassword 新密码
     */
    Q_INVOKABLE void changePassword(QString oldPassword, QString newPassword);

    /*! \fn QString getUserId()
     *  \brief 返回当前登录用户的用户名
     *  \return 当前登录用户的用户名
     */
    Q_INVOKABLE QString getUserId() const;

    // 0 for Customer, 1 for Seller

    /*! \fn bool registerUser(int userType, QString userid, QString password, float balance)
     *  \brief 注册用户，程序会检查是否已经存在该用户
     *  \param userType 用户类型，输入'0'表示消费者，'1'表示商家
     *  \param userid 用户名
     *  \param password 密码
     *  \param balance 用户初始余额，默认为'0'
     *  \return 用返回值表示是否注册成功，为true表示成功，为false表示错误
     */
    Q_INVOKABLE bool registerUser(int userType, QString userid, QString password, float balance = 0);

    /*! \fn void buyCommodity(QString name, QString user, float price, int num)
     *  \brief 购买商品，消费余额为当前已登录用户的余额
     *  \param name 商品名
     *  \param user 商品的商家
     *  \param price 商品价格
     *  \param num 购买数量
     */
    Q_INVOKABLE void buyCommodity(QString name, QString user, float price, int num);

    /*! \fn void recharge(float n_balance)
     *  \brief 给当前用户充值，该函数用于与界面类交互
     *  \param n_balance 要充值的金额大小
     */
    Q_INVOKABLE void recharge(float n_balance);

    /*! \fn void changeCommodity(QString name, QString description, QString user, QString price, QString stored)
     *  \brief 修改商品信息
     *  \param name 要修改的商品名
     *  \param description 新的商品描述
     *  \param user 修改商品的对应商家
     *  \param price 新的商品价格
     *  \param stored 新的商品库存
     */
    Q_INVOKABLE void changeCommodity(QString name, QString description, QString user, QString price, QString stored);

    /*! \fn void deleteCommodity(QString name, QString user)
     *  \brief 下架商品，商家只能下架自己的商品
     *  \param name 要下架的商品名
     *  \param use r 下架商品的对应商家
     */
    Q_INVOKABLE void deleteCommodity(QString name, QString user);

    /*! \fn void void addEvent(QString type, float percent)
     *  \brief 对当前已登录用户的商品添加折扣信息
     *  \param type 要添加折扣的商品类型
     *  \param percent 折扣的比例，打折后商品的价格会乘以该系数
     */
    Q_INVOKABLE void addEvent(QString type, float percent);

signals:
    void signIn(bool isCustomer);
    void signOut();
    void updateData(BaseCommodity* list);
    void infoHappen(QString msg);
    void errorHappen(QString err);
    void balanceChange(float n_balance);
    void buyCommoditySuccess();
    void changeCommoditySuccess();
    void deleteCommoditySuccess();

private:
    BaseUser* currentUser;  //!< 当前客户端登录的用户，未登录时为空
    // curCustomerUser和curSellerUser同时只能存在一个
    CustomerUser *curCustomerUser;  //!< 当前客户端登录的消费者用户，未登录时为空
    SellerUser *curSellerUser;  //!< 当前客户端登录的商家用户，未登录时为空

    QSqlDatabase database; //!< 客户端的数据库对象
    QString configFile; //!< 客户端用户信息的文件路径
    QList<CustomerUser *> customerList; //!< 加载到内存中的消费者用户列表
    QList<SellerUser *> sellerList; //!< 加载到内存中的商家用户列表

public slots:

};

#endif // CLIENT_H
