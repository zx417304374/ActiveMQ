#ifndef CONSUMER_H
#define CONSUMER_H

#include <QThread>
#include <QObject>
#include <QDebug>

#include <activemq/library/ActiveMQCPP.h>
#include <decaf/lang/Thread.h>
#include <decaf/lang/Runnable.h>
#include <decaf/util/concurrent/CountDownLatch.h>
#include <decaf/lang/Integer.h>
#include <decaf/lang/Long.h>
#include <decaf/lang/System.h>
#include <activemq/core/ActiveMQConnectionFactory.h>
#include <activemq/core/ActiveMQConnection.h>
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
#include "ActiveMQTransportListener.h"

using namespace activemq::core;
using namespace decaf::util::concurrent;
using namespace decaf::util;
using namespace decaf::lang;
using namespace cms;
using namespace std;


class Consumer : public QObject
               , public ExceptionListener
               , public MessageListener
               , public Runnable
{
    Q_OBJECT
private:
    ActiveMQConnection* m_connection;
    Session* m_session;
    Destination* m_destination;
    MessageConsumer* m_consumer;
    bool m_quit;
    bool m_useTopic;
    bool m_sessionTransacted;
    bool m_isTopicPersistent;
    std::string m_brokerURI;
    std::string m_name;
    std::string m_username;
    std::string m_password;
    std::string m_clientId;
    std::string m_subscriptionName;
    ActiveMQTransportListener* m_MQTransportListener;
private:

    Consumer(const Consumer&);
    Consumer& operator=(const Consumer&);

public:

    Consumer(const std::string& name,const std::string& username,
             const std::string& password,const std::string& brokerURI,
             const std::string& aCilentId, const std::string& aSubscriptionName,
             bool aIsTopicPersistent = false,
             bool useTopic = false,
             bool sessionTransacted = false)
        : m_connection(NULL)
        , m_session(NULL)
        , m_quit(false)
        , m_destination(NULL)
        , m_consumer(NULL)
        , m_useTopic(useTopic)
        , m_sessionTransacted(sessionTransacted)
        , m_isTopicPersistent(aIsTopicPersistent)
        , m_brokerURI(brokerURI)
        , m_name(name)
        , m_username(username)
        , m_password(password)
        , m_clientId(aCilentId)
        , m_subscriptionName(aSubscriptionName)
        , m_MQTransportListener(NULL)
    {
        qDebug() << __FUNCTION__ << __LINE__
                 << QString::fromStdString(m_name)
                 << QString::fromStdString(m_username)
                 << QString::fromStdString(m_password)
                 << QString::fromStdString(m_brokerURI)
                 << QString::fromStdString(m_clientId)
                 << QString::fromStdString(m_subscriptionName)
                 << m_useTopic
                 << m_sessionTransacted
                 << m_isTopicPersistent;
    }

    virtual ~Consumer() {
        cleanup();
    }

    void setquit()
    {
        m_quit = true;
    }

    void close() {
        this->cleanup();
    }

    void waitUntilReady() {
    }

    virtual void run();

    // Called from the consumer since this class is a registered MessageListener.
    virtual void onMessage(const Message* message);

    // If something bad happens you see it here as this class is also been
    // registered as an ExceptionListener with the connection.
    virtual void onException(const CMSException& ex AMQCPP_UNUSED);

private:
    void cleanup();

signals:
    void messageReceived(const QString& aMessage);
    void MQStateChanged(MQState aState);
};

#endif // CONSUMER_H
