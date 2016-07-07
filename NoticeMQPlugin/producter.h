#ifndef PRODUCTER_H
#define PRODUCTER_H

#include <activemq/library/ActiveMQCPP.h>
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
#include <QObject>
#include <QMap>

using namespace activemq::core;
using namespace decaf::util::concurrent;
using namespace decaf::util;
using namespace decaf::lang;
using namespace cms;
using namespace std;

class Producter : public QObject,
                  public ExceptionListener,
                  public MessageListener
{
    Q_OBJECT
private:

    Producter(const Producter&);
    Producter& operator=(const Producter&);

public:
    Producter(const std::string& name,const std::string& username,
              const std::string& password,const std::string& brokerURI);
    ~Producter();

public:
    // Called from the consumer since this class is a registered MessageListener.
    virtual void onMessage(const Message* message);

    // If something bad happens you see it here as this class is also been
    // registered as an ExceptionListener with the connection.
    virtual void onException(const CMSException& ex AMQCPP_UNUSED);

public:
    void cleanup();
    void sendMsg(const QString &aMsg);

private:
    void getRandString(QString & randString);

signals:
    void messageReceived(const QString& aMessage);

private:
    Connection* m_connection;
    Session* m_session;
    Destination* m_destination;
    MessageProducer* m_producer;
    std::string m_brokerURI;
    std::string m_name;
    std::string m_username;
    std::string m_password;

    shared_ptr<ConnectionFactory> m_connectionFactory;

    TemporaryQueue* m_tempDest;
    //临时队列名称，对应消费者
    QMap<QString,MessageConsumer*> m_responseConsumerMap;
    TextMessage* m_txtMessage;
};

#endif // PRODUCTER_H
