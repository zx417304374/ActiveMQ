#include "NoticeMQPlugin.h"
#include <QDateTime>
#include <QDebug>
#include <QSettings>
#include <QCoreApplication>
//#ifdef _DEBUG
//#include "vld.h"
//#endif


NoticeMQPlugin::NoticeMQPlugin(QObject *parent)
    : Interface()
    , m_consumer(NULL)
    , m_producter(NULL)
    , m_consumerThread(NULL)
    , m_useTopic(false)
    , m_sessionTransacted(false)
    , m_isTopicPersistent(false)
    , m_brokerURI()
    , m_topicname()
    , m_username()
    , m_password()

{
    activemq::library::ActiveMQCPP::initializeLibrary();
    //为了最外层能获取是哪个MQ发出的消息必须设置objectName
    setObjectName("NoticeMQ");
    LoadConfig();
//    connect(this, SIGNAL(MqLibClosed()),
//            this, SLOT(closeMQLib()), Qt::QueuedConnection);

    Q_UNUSED(parent);
}
NoticeMQPlugin::~NoticeMQPlugin()
{
    if (NULL != m_consumer)
    {
    try {
            if(NULL == m_consumerThread)
        {
                return ;
            }
            m_consumer->close();
            delete m_consumer;
            m_consumer = NULL;
//            m_consumerThread->join();
//            delete m_consumerThread;
//            m_consumerThread = NULL;

        } catch (CMSException& e) {
            qDebug() << __FUNCTION__ << __LINE__;
            e.printStackTrace();
    }
    }
//    endConsumer();
    activemq::library::ActiveMQCPP::shutdownLibrary();
    qDebug()<<">> TEST INFO:"<<__FUNCTION__<<__LINE__;
}

void NoticeMQPlugin::LoadConfig()\
{
    QString pluginPath = QCoreApplication::applicationDirPath() + "/Plugin/" +  objectName();
    QString pluginName = pluginPath + "/" + objectName() + "Plugin.ini";
    qDebug()<<">> TEST INFO:"<<__FUNCTION__<<__LINE__ << pluginName;

    QSettings settings(pluginName,QSettings::IniFormat);
    m_brokerURI = settings.value("brokerURI","failover:(tcp://localhost:61616/)").toString().toStdString();
    m_useTopic = settings.value("useTopics",false).toBool();
    m_sessionTransacted = settings.value("sessionTransacted",false).toBool();
    m_isTopicPersistent = settings.value("persistent",false).toBool();
    m_topicname = settings.value("topicname","topic://NoticeTopic").toString().toStdString();
    m_queuename = settings.value("queuename","queue://NoticeQueue").toString().toStdString();
    m_username = settings.value("username","").toString().toStdString();
    m_password = settings.value("password","").toString().toStdString();

}


/*
 * QVariantMap参数规则
 * 入参格式：
 * FuncName：函数名
 * ArgCounts：参数个数
 * Arg1：第一个参数
 * Arg2：第二个参数
 * 。
 * 。
 * 。
 * 以此类推(目前对9个参数的函数进行了处理)
 * *************************
 * 出参格式
 * code : 返回值
 * data : 卡信息
 * error: 错误信息
*/
QVariantMap NoticeMQPlugin::callFunc(QVariantMap &aMap)
{
    m_outData.clear();
    qDebug() << __FUNCTION__ << __LINE__ ;
    bool ok = true;
    QString funcName = aMap.value("FuncName").toString();
    int argCounts = aMap.value("ArgCounts").toInt(&ok);
    if (!ok)
    {
        m_outData["error"] = /*QStringLiteral*/("参数个数错误, 请输入整形.");
        return m_outData;
    }
    QList<QString> argList;
    for(int i = 1; i <= argCounts; ++i)
    {
        QString argKey("Arg%1");
        argList.append(aMap.value(argKey.arg(i)).toString());
    }
    if (0 == argCounts)
    {
        QMetaObject::invokeMethod(this, funcName.toLatin1().data(), Qt::DirectConnection,
                                  Q_RETURN_ARG(QVariantMap , m_outData));

    }
    else if (1 == argCounts)
    {
        QMetaObject::invokeMethod(this, funcName.toLatin1().data(), Qt::DirectConnection,
                                  Q_RETURN_ARG(QVariantMap , m_outData),
                                  Q_ARG(QString, aMap.value("Arg1").toString()));

    }

    else if (2 == argCounts)
    {
        QMetaObject::invokeMethod(this, funcName.toLatin1().data(), Qt::DirectConnection,
                                  Q_RETURN_ARG(QVariantMap , m_outData),
                                  Q_ARG(QString, aMap.value("Arg1").toString()),
                                  Q_ARG(QString, aMap.value("Arg2").toString()));
    }
    else
    {
        m_outData["error"] = /*QStringLiteral*/("暂无对应的方法");
    }
    return m_outData;
}

/*
 *启动consumer
 * 入参：
 *      aBorkerURL：服务器url
 *      aChannelName： 通道名称
 *      aUserName：用户名称
 *      aPassword：用户密码
 *      aClientId: 订阅ID
 *      aSubName : 订阅名称
 * 返回值：
 *      retMap["code"] == 0 成功 其余失败
 *
*/
QVariantMap NoticeMQPlugin::startConsumer(const QString& aClientId, const QString& aSubName)
{
    QVariantMap retMap;
    std::string clientId = aClientId.toStdString();
    std::string subName = aSubName.toStdString();


    m_consumer = new Consumer(m_topicname,m_username,m_password, m_brokerURI,
                              clientId, subName,m_isTopicPersistent,
                              m_useTopic, m_sessionTransacted);
    disconnect(m_consumer, SIGNAL(messageReceived(const QString&)),
                this, SLOT(reveivedConMessage(const QString& )));
    disconnect(m_consumer, SIGNAL(MQStateChanged(MQState)),
               this, SLOT(changeMQState(MQState)));

    connect(m_consumer, SIGNAL(messageReceived(const QString&)),
            this, SLOT(reveivedConMessage(const QString& )));
    connect(m_consumer, SIGNAL(MQStateChanged(MQState)),
            this, SLOT(changeMQState(MQState)));
    qDebug() << __FUNCTION__ << __LINE__ << "m_consumerThread == NULL";
    m_consumerThread = new Thread(m_consumer);
    m_consumerThread->start();
    qDebug() << __FUNCTION__ << __LINE__ << QStringLiteral("emit MQStateChanged信号");
    retMap["code"] = 0;
    return retMap;
}

QVariantMap NoticeMQPlugin::endConsumer()
{
    QVariantMap retMap;
    if (NULL != m_consumer)
    {
    try{
        qDebug() << __FUNCTION__ << __LINE__;
        if(NULL == m_consumerThread)
        {
            retMap["code"] = -1;
            return retMap;
        }
            m_consumer->close();
            delete m_consumer;
            m_consumer = NULL;

//            m_consumerThread->join();
            qDebug() << __FUNCTION__ << __LINE__  << "Thread isAlive:"<< m_consumerThread->isAlive();
        delete m_consumerThread;
        m_consumerThread = NULL;

            qDebug() << __FUNCTION__ << __LINE__  << "end interrupt Thread";

    } catch (CMSException& e) {
        qDebug() << __FUNCTION__ << __LINE__;
        e.printStackTrace();
    }
    }
    retMap["code"] = 0;
    return retMap;
}

QVariantMap NoticeMQPlugin::sendMsgToServer(const QString& aMessage)
{
    QVariantMap retMap;
    //Test SendMsg Code begin
    qDebug()<<">> TEST INFO:"<<__FUNCTION__<<__LINE__;
    if(NULL == m_producter)
    {
        m_producter = new Producter(m_queuename, m_username, m_password, m_brokerURI);
        connect(m_producter, SIGNAL(messageReceived(const QString&)),
                this, SLOT(reveivedProMessage(const QString& )),Qt::QueuedConnection);
    }
//    m_QThread = new QThread();
//    m_producter->moveToThread(m_QThread);
    m_producter->sendMsg(aMessage);

    //Test SendMsg Code end
    retMap["code"] = 0;
    return retMap;
}

//接受到来自生产者的消息
void NoticeMQPlugin::reveivedConMessage(const QString& aMessage)
{
    qDebug() << __FUNCTION__ <<__LINE__ << "Reveived Message : " << aMessage;
    emit MQConsumerMsgReceived(objectName(), aMessage);
}

//接受到来自消费者的消息
void NoticeMQPlugin::reveivedProMessage(const QString& aMessage)
{
    qDebug() << __FUNCTION__ <<__LINE__ << "Reveived Message : " << aMessage;
    emit MQProducterMsgReceived(objectName(), aMessage);
    if(NULL != m_producter)
    {
        disconnect(m_producter, SIGNAL(messageReceived(const QString&)),
                    this, SLOT(reveivedProMessage(const QString& )));
        delete m_producter;
        m_producter = NULL;
    }
}

void NoticeMQPlugin::changeMQState(MQState aState)
{
    if (aState == MQCLOSE)
    {
        m_consumerThread->join();
    }
    emit MQStateChanged(objectName(), aState);
}

////由于Thread退出需要时间，因此通过信号延时关闭MQ
//void NoticeMQPlugin::closeMQLib()
//{
//    qDebug() << __FUNCTION__ <<__LINE__ ;
//    delete m_consumerThread;
//    m_consumerThread = NULL;
//}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(NoticeMQPlugin, NoticeMQPlugin);
#endif // QT_VERSION < 0x050000


