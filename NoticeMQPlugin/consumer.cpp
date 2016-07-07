#include "consumer.h"
#include <QTime>

#include <QDebug>
void Consumer::run()
{
    try {

        // Create a ConnectionFactory
        auto_ptr<ConnectionFactory> connectionFactory(
                    ConnectionFactory::createCMSConnectionFactory(m_brokerURI));

        qDebug() << __FUNCTION__ << __LINE__ << "after connectionFactory";

        // Create a Connection
        try {
        m_connection = dynamic_cast<ActiveMQConnection*>(connectionFactory->createConnection(m_username,m_password));
        } catch (cms::CMSException& ex) {
            qDebug() << __FUNCTION__ << __LINE__ << "URI Error";
            ex.printStackTrace();
            emit MQStateChanged(MQERROR);
            return;
        }
        qDebug() << __FUNCTION__ << __LINE__ << "after createConnection";
        m_MQTransportListener = new ActiveMQTransportListener();
        disconnect(m_MQTransportListener, SIGNAL(ActiveMQStateChanged(MQState)),
                   this, SIGNAL(MQStateChanged(MQState)));
        connect(m_MQTransportListener, SIGNAL(ActiveMQStateChanged(MQState)),
                this, SIGNAL(MQStateChanged(MQState)));
        qDebug() << __FUNCTION__ << __LINE__ << "before use m_connection";
//        if (m_connection->isStarted())
//        {
//            qDebug() << __FUNCTION__ << __LINE__ << "m_connection Transport error";
//            emit MQStateChanged(MQERROR);
//            return ;
//        }
        qDebug() << __FUNCTION__ << __LINE__ << "m_connection Transport right";
//        m_connection->setCloseTimeout(5000);
        if(m_isTopicPersistent)
        {
            m_connection->setClientID(m_clientId);
            qDebug() << __FUNCTION__ << __LINE__  << "after setClicentId";
        }
//        if (m_connection->isCheckForDuplicates())
        m_connection->start();
        m_connection->addTransportListener(m_MQTransportListener);
        qDebug() << __FUNCTION__ << __LINE__ << "after m_connection start";

        // Create a Session
        if (m_sessionTransacted) {
            m_session = m_connection->createSession(Session::SESSION_TRANSACTED);
        } else {
            m_session = m_connection->createSession(Session::AUTO_ACKNOWLEDGE);
        }

        // Create the destination (Topic or Queue)
        QString selectorStr = QString("U%1='C%2'").arg(QString::fromStdString(m_subscriptionName)).arg(QString::fromStdString(m_subscriptionName));
        //默认持久化
        if(m_isTopicPersistent)
        {
            m_destination = m_session->createTopic(m_name);
            m_consumer = m_session->createDurableConsumer((Topic *)m_destination,m_subscriptionName,selectorStr.toStdString(),true);
        }
        else
        {
            if(m_useTopic)
                m_destination = m_session->createTopic(m_name);
            else
                m_destination = m_session->createQueue(m_name);
            // Create a MessageConsumer from the Session to the Topic or Queue
            m_consumer = m_session->createConsumer(m_destination,selectorStr.toStdString());
        }


        m_consumer->setMessageListener(this);
        qDebug() << __FUNCTION__ << __LINE__ << "after m_consumer run";
        emit MQStateChanged(MQOPEN);

//        while (!m_quit) {

//        }
    } catch (CMSException& e) {
        e.printStackTrace();
    }
}

void Consumer::onMessage(const Message* message)
{
    qDebug()<<"Consumer::onMessage";
    try {
        const TextMessage* textMessage = dynamic_cast<const TextMessage*> (message);
        string text = "";

        if (textMessage != NULL) {
            text = textMessage->getText();
        } else {
            text = "NOT A TEXTMESSAGE!";
        }

        qDebug() << QString::fromUtf8(text.c_str());
        emit messageReceived(QString::fromUtf8(text.c_str()));

    } catch (CMSException& e) {
        e.printStackTrace();
    }

    // Commit all messages.
    if (this->m_sessionTransacted) {
        m_session->commit();
    }
}

void Consumer::onException(const CMSException& ex AMQCPP_UNUSED)
{
    printf("CMS Exception occurred.  Shutting down client.\n");
//    m_main->recvMsgfromSuperHub(QString::fromLocal8Bit("状态"),QString::fromLocal8Bit("程序错误"));
    emit MQStateChanged(MQERROR);
    ex.printStackTrace();
    exit(1);
}

void Consumer::cleanup() {
    qDebug()<<">> TEST INFO:"<<__FUNCTION__<<__LINE__;
//    m_main->recvMsgfromSuperHub(QString::fromLocal8Bit("状态"),QString::fromLocal8Bit("MQ关闭"));
    if (m_connection != NULL) {
        try {
            m_session->close();
            m_connection->close();
            qDebug()<<">> TEST INFO:"<<__FUNCTION__<<__LINE__;
        } catch (cms::CMSException& ex) {
            ex.printStackTrace();
        }
    // Destroy resources.
    try {
            qDebug()<<">> TEST INFO:"<<__FUNCTION__<<__LINE__;
        delete m_destination;
        m_destination = NULL;
        delete m_consumer;
        m_consumer = NULL;
            qDebug()<<">> TEST INFO:"<<__FUNCTION__<<__LINE__;
        delete m_session;
        m_session = NULL;
        delete m_connection;
        m_connection = NULL;
        delete m_MQTransportListener;
        m_MQTransportListener = NULL;
    } catch (CMSException& e) {
        e.printStackTrace();
    }
        emit MQStateChanged(MQCLOSE);
        qDebug()<<">> TEST INFO:"<<__FUNCTION__<<__LINE__ << "emit closeSignal";
    }
    qDebug()<<">> TEST INFO:"<<__FUNCTION__<<__LINE__ << "end cleanup ";
}
