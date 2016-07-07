#ifndef NoticeMQPLUGIN_H
#define NoticeMQPLUGIN_H
#include <qplugin.h>
#include <QThread>
#include <QVariantMap>
#include <activemq/library/ActiveMQCPP.h>
#include <decaf/lang/Thread.h>
#include <decaf/lang/Runnable.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <decaf/lang/Integer.h>
#include <decaf/lang/Long.h>
#include <decaf/lang/System.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/util/Config.h>
#include <cms/Connection.h>
#include <cms/Session.h>
#include <cms/TextMessage.h>
#include <cms/BytesMessage.h>
#include <cms/MapMessage.h>
#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <memory>
#include <QThread>
#include "consumer.h"
#include "producter.h"
#include "Interface.h"

using namespace activemq::core;
using namespace decaf::util::concurrent;
using namespace decaf::util;
using namespace decaf::lang;
using namespace cms;
using namespace std;


class NoticeMQ;

class NoticeMQPlugin : public Interface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QGenericPluginFactoryInterface" FILE "NoticeMQPlugin.json")
#endif // QT_VERSION >= 0x050000
    Q_INTERFACES(Interface)

public:
    NoticeMQPlugin(QObject *parent = 0);
    ~NoticeMQPlugin();

private:
    void LoadConfig();
signals:

public slots:
    QVariantMap callFunc(QVariantMap& aMap);

    QVariantMap startConsumer(const QString& aClientId, const QString &aSubName);
    QVariantMap endConsumer();
    QVariantMap sendMsgToServer(const QString& aMessage);
    void reveivedConMessage(const QString& aMessage);
    void reveivedProMessage(const QString& aMessage);
    void changeMQState(MQState aState);

//signals:
//    void MqLibClosed();
//private slots:
//    void closeMQLib();

private:
    QVariantMap m_outData;
    Consumer*   m_consumer;
    Producter*  m_producter;
    Thread*     m_consumerThread;

    //从ini获取数据，避免函数多参数
    bool        m_useTopic;
    bool        m_sessionTransacted;
    bool        m_isTopicPersistent;
    std::string m_brokerURI;
    std::string m_topicname;
    std::string m_queuename;
    std::string m_username;
    std::string m_password;
//    QThread* m_QThread;
};

#endif // NoticeMQPLUGIN_H
