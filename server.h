#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QWidget>
#include <QFile>
#include <QDataStream>
#include <QTcpSocket>
#include <QTcpServer>
#include <QList>
#include <QJsonArray>
#include <QSqlDatabase>
#include <QSqlQuery>
#include "thread.h"
#include "../eco/eco/lib/commodity.h"
#include "../eco/eco/lib/user.h"

struct UserResult {
    QJsonObject userinfo;
    UserResponseType type;
    BaseUser* user;
};

class Server : public QWidget
{
    Q_OBJECT
public:
    explicit Server(QWidget *parent = nullptr);
    ~Server() override;
    Thread* find_socket(unsigned char p_id);

    void setUI();

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

    /*! \fn void loginUser(QString userid, QString password)
     *  \brief 登录用户，登陆成功后将会设置客户端用户
     *  \param userid 用户名
     *  \param password 密码
     *  \return 表示是否登陆成功，为true表示成功，为false表示错误
     */
    UserResult loginUser(QString userid, QString password);

    /*! \fn bool registerUser(int userType, QString userid, QString password, float balance)
     *  \brief 注册用户，程序会检查是否已经存在该用户
     *  \param userType 用户类型，输入'0'表示消费者，'1'表示商家
     *  \param userid 用户名
     *  \param password 密码
     *  \return 用返回值表示是否注册成功，为true表示成功，为false表示错误
     */
    UserResponseType registerUser(int userType, QString userid, QString password);


public slots:
    void newConnection();
    void sqlCommandExec(unsigned char p_id, Command command);
    void userCommandExec(unsigned char p_id, Command command);
    void eventCommandExec(unsigned char p_id, Command command);
    void aboutToQuit();
    void closeThread(Thread* thread);


signals:

private:
    void openDataBase();


    unsigned char nthread;
    QTcpServer* server;
    QList<Thread*> threadPool;
    QSqlDatabase database;
    QList<CustomerUser *> customerList; //!< 加载到内存中的消费者用户列表
    QList<SellerUser *> sellerList; //!< 加载到内存中的商家用户列表
};

#endif // SERVER_H
