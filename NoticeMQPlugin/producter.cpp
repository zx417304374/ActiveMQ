#include "producter.h"
#include <QDebug>
#include <QTime>

Producter::Producter(const string &name, const string &username,
                     const string &password, const string &brokerURI)
    : m_connection(NULL)
    , m_session(NULL)
    , m_destination(NULL)
    , m_producer(NULL)
    , m_brokerURI(brokerURI)
    , m_name(name)
    , m_username(username)
    , m_password(password)
    , m_connectionFactory(shared_ptr<ConnectionFactory>(ConnectionFactory::createCMSConnectionFactory(m_brokerURI)))
    , m_tempDest(NULL)
{
    qDebug() << __FUNCTION__ << __LINE__ << "Producter";
    m_connection = m_connectionFactory->createConnection(m_username, m_password);
//    m_connection = m_connectionFactory->createConnection();
    m_connection->start();
    m_session = m_connection->createSession(Session::AUTO_ACKNOWLEDGE);
    m_destination = m_session->createQueue(m_name);
    m_producer = m_session->createProducer(m_destination);
    m_responseConsumerMap.clear();
}

Producter::~Producter()
{
    cleanup();
}

void Producter::sendMsg(const QString& aMsg)
{
    qDebug()<<">> TEST INFO:"<<__FUNCTION__<<__LINE__;

    // Create a Connection

    try {
        m_producer->setDeliveryMode(DeliveryMode::NON_PERSISTENT);

        m_tempDest = m_session->createTemporaryQueue();
        MessageConsumer* responseConsumer = m_session->createConsumer(m_tempDest);
        responseConsumer->setMessageListener(this);
        QString queueName = QString::fromStdString(m_tempDest->getQueueName());
        if (!m_responseConsumerMap.keys().contains(queueName))
        {
            m_responseConsumerMap.insert(queueName, responseConsumer);
        }
        //Now create the actual message you want to send
        m_txtMessage = m_session->createTextMessage();
        m_txtMessage->setText(aMsg.toStdString());

        m_txtMessage->setCMSReplyTo(m_tempDest);

        QString randStr;
        getRandString(randStr);
        m_txtMessage->setCMSCorrelationID(randStr.toUtf8().data());
        m_producer->send(m_txtMessage);
//        TextMessage* textMessage = dynamic_cast<TextMessage*>(m_responseConsumer->receive());
//        string text = "";

//        if (textMessage != NULL) {
//            text = textMessage->getText();
//        } else {
//            text = "NOT A TEXTMESSAGE!";
//            return;
//        }

//        qDebug()<<">> TEST INFO:"<<__FUNCTION__<<__LINE__ << QString::fromUtf8(text.c_str());
//        emit messageReceived(QString::fromUtf8(text.c_str()));

        delete m_txtMessage;
        m_txtMessage = NULL;
//        m_responseConsumer->close();
//        delete m_responseConsumer;
//        m_responseConsumer = NULL;
        delete m_tempDest;
        m_tempDest = NULL;

    } catch (CMSException e) {
        //Handle the exception appropriately
    }
}
void Producter::getRandString(QString & randString)
{
    int max = 16;
    QString tmp = QString("0123456789ABCDEFGHIJKLMNOPQRSTUVWZYZ");
    QString str = QString();
    QTime t;
    t= QTime::currentTime();
    qsrand(t.msec()+t.second()*1000);
    for(int i=0;i<max;i++)
    {
        int ir = qrand()%tmp.length();
        str[i] = tmp.at(ir);
    }
    randString = str;
}

void Producter::onMessage(const Message* message)
{
    qDebug()<<">> TEST INFO:"<<__FUNCTION__<<__LINE__;
    try {
        const TextMessage* textMessage = dynamic_cast<const TextMessage*> (message);
        string text = "";

        if (textMessage != NULL) {
            text = textMessage->getText();
        } else {
            text = "NOT A TEXTMESSAGE!";
            return;
        }

        qDebug()<<">> TEST INFO:"<<__FUNCTION__<<__LINE__ << QString::fromUtf8(text.c_str());
        emit messageReceived(QString::fromUtf8(text.c_str()));
        const TemporaryQueue* tmp = dynamic_cast<const TemporaryQueue*>(textMessage->getCMSReplyTo());
        if (tmp != NULL)
        {
            QString queueName = QString::fromStdString(tmp->getQueueName());

            if (m_responseConsumerMap.keys().contains(queueName))
            {
                qDebug()<<">> TEST INFO:"<<__FUNCTION__<<__LINE__<< queueName;
                MessageConsumer* messgaeConsumer = m_responseConsumerMap.value(queueName);
                delete messgaeConsumer;
                messgaeConsumer = NULL;
                m_responseConsumerMap.remove(queueName);
            }

        }

    } catch (CMSException& e) {
        e.printStackTrace();
    }

}

void Producter::onException(const CMSException& ex AMQCPP_UNUSED)
{
    printf("CMS Exception occurred.  Shutting down client.\n");
//    m_main->recvMsgfromSuperHub(QString::fromLocal8Bit("×´Ì¬"),QString::fromLocal8Bit("³ÌÐò´íÎó"));
    ex.printStackTrace();
    exit(1);
}

void Producter::cleanup() {
//    m_main->recvMsgfromSuperHub(QString::fromLocal8Bit("×´Ì¬"),QString::fromLocal8Bit("MQ¹Ø±Õ"));
    if (m_connection != NULL) {
        try {
            m_connection->close();
        } catch (cms::CMSException& ex) {
            ex.printStackTrace();
        }
    }

    // Destroy resources.
    try {
        delete m_destination;
        m_destination = NULL;
        delete m_producer;
        m_producer = NULL;
        delete m_session;
        m_session = NULL;
        delete m_connection;
        m_connection = NULL;
        foreach(MessageConsumer* consumer, m_responseConsumerMap)
        {
            consumer->close();
            delete consumer;
            consumer = NULL;
        }
        m_responseConsumerMap.clear();
    } catch (CMSException& e) {
        e.printStackTrace();
    }
    qDebug()<<">> TEST INFO:"<<__FUNCTION__<<__LINE__;
}
