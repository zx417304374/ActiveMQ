#include "ActiveMQTransportListener.h"
#include <QDebug>

ActiveMQTransportListener::ActiveMQTransportListener(QObject *parent)
  : QObject(parent)
{
    qDebug() << __FUNCTION__ << __LINE__;
}

void ActiveMQTransportListener::onCommand(const Pointer<Command> command)
{
    Q_UNUSED(command);

}


void ActiveMQTransportListener::onException(const decaf::lang::Exception& ex)
{
    Q_UNUSED(ex);
    qDebug() << __FUNCTION__ << __LINE__;
    emit ActiveMQStateChanged(MQERROR);
}

void ActiveMQTransportListener::transportInterrupted()
{
    qDebug() << __FUNCTION__ << __LINE__;
    emit ActiveMQStateChanged(MQINTERRUPTED);
}
void ActiveMQTransportListener::transportResumed()
{
    qDebug() << __FUNCTION__ << __LINE__;
    emit ActiveMQStateChanged(MQRESUMED);

}
