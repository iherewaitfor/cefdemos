#ifndef _DYNAMICCLIENTTREEHELPER_H
#define _DYNAMICCLIENTTREEHELPER_H

#include <QObject>
#include <QSharedPointer>

#include <QRemoteObjectNode>
#include <qremoteobjectdynamicreplica.h>
#include<QRemoteObjectPendingCallWatcher>
#include "dynamicclient.h"

class DynamicClientTreeHelper : public QObject
{
    Q_OBJECT
public:
    DynamicClientTreeHelper(QSharedPointer<QRemoteObjectDynamicReplica> ptr);
    ~DynamicClientTreeHelper() override;
    const QMap<QString, QSharedPointer<QRemoteObjectDynamicReplica>>& getObjectsMap();
public Q_SLOTS:
    void initConnection_slot(); //QRemoteObjectDynamicReplica
    void stateChanged_slot(QRemoteObjectReplica::State state, QRemoteObjectReplica::State oldState);
    void pendingCallResult(QRemoteObjectPendingCallWatcher* self);
private:
    QSharedPointer<QRemoteObjectDynamicReplica> reptr;// holds reference to replica
    bool m_binitConnection;
    QMap<QString, QSharedPointer<QRemoteObjectDynamicReplica>> m_objectsMap;
    QMap<QString, QSharedPointer<DynamicClient>> m_dynamicClientsMap;
 };

#endif

