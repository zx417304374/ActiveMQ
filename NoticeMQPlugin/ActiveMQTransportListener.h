#ifndef ACTIVEMQTRANSPORTLISTENER_H
#define ACTIVEMQTRANSPORTLISTENER_H

#include <QObject>
#include <activemq/transport/TransportListener.h>
#include "Interface.h"

using namespace activemq::transport;

class ActiveMQTransportListener : public QObject,
                                  public TransportListener
{
    Q_OBJECT
public:
    explicit ActiveMQTransportListener(QObject *parent = 0);


    virtual void onCommand(const Pointer<Command> command);

    /**
     * Event handler for an exception from a command transport.
     *
     * @param ex
     *      The exception being propagated to this listener to handle.
     */
    virtual void onException(const decaf::lang::Exception& ex);

    /**
     * The transport has suffered an interruption from which it hopes to recover
     */
    virtual void transportInterrupted();

    /**
     * The transport has resumed after an interruption
     */
    virtual void transportResumed();

signals:
    void ActiveMQStateChanged(MQState aState);

public slots:
};

#endif // ACTIVEMQTRANSPORTLISTENER_H
