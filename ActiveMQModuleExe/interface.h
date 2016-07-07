#ifndef INTERFACE_H
#define INTERFACE_H
#include <QObject>
#include <QVariantMap>

enum MQState
{
    MQOPEN = 1,
    MQERROR = 2,
    MQINTERRUPTED = 3,
    MQRESUMED = 4,
    MQCLOSE = 5
};

class Interface: public QObject
{
    Q_OBJECT
    Q_ENUMS(MQState)
public:
    virtual ~Interface() {}

public slots:
    virtual QVariantMap callFunc(QVariantMap& aMap) = 0;

signals:
    void MQConsumerMsgReceived(const QString& aCurrentMQName, const QString& aMQMessage);
    void MQProducterMsgReceived(const QString& aCurrentMQName, const QString& aMQMessage);
    void MQStateChanged(const QString& aCurrentMQName, MQState aState);
};

Q_DECLARE_INTERFACE(Interface,"workpf.epsoft.com/1.0")

#endif // INTERFACE_H
