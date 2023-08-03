#ifndef _DYNAMICCLIENT_H
#define _DYNAMICCLIENT_H

#include <QObject>
#include <QSharedPointer>
#include <qremoteobjectdynamicreplica.h>

class DynamicClient : public QObject
{
    Q_OBJECT
public:
    DynamicClient(QSharedPointer<QRemoteObjectDynamicReplica> ptr);
    ~DynamicClient() override;
    QSharedPointer<QRemoteObjectDynamicReplica> getReplica();
public Q_SLOTS:
    void initConnection_slot(); //QRemoteObjectDynamicReplica
    void stateChanged_slot(QRemoteObjectReplica::State state, QRemoteObjectReplica::State oldState);
private:
    bool clientSwitchState; // holds received server switch state
    QSharedPointer<QRemoteObjectDynamicReplica> reptr;// holds reference to replica
    bool m_binitConnection;
 };

#endif

