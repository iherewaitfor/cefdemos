#ifndef _DYNAMICCLIENTTREEHELPER_H
#define _DYNAMICCLIENTTREEHELPER_H

#include <QObject>
#include <QSharedPointer>

#include <QRemoteObjectNode>
#include <qremoteobjectdynamicreplica.h>
#include<QRemoteObjectPendingCallWatcher>
#include "dynamicclient.h"
#include "../qcefipcvalue.h"

#include "../qcefv8bindroapp.h"
#include "../qcefv8bindroapp_p.h"

class DynamicClientTreeHelper : public QObject
{
    Q_OBJECT
public:
    DynamicClientTreeHelper(QSharedPointer<QRemoteObjectDynamicReplica> ptr);
    ~DynamicClientTreeHelper() override;
    const QMap<QString, QSharedPointer<DynamicClient>>& getObjectsMap();
    void getMetaObjects(QList<cefv8bind_protcool::CefMetaObject>& cef_metaObjects);
public Q_SLOTS:
    void initConnection_slot(); //QRemoteObjectDynamicReplica
    void stateChanged_slot(QRemoteObjectReplica::State state, QRemoteObjectReplica::State oldState);
    void pendingCallResult(QRemoteObjectPendingCallWatcher* self);
    //render
    void contextCreated_slot(int browserId, qint64 frameId);
Q_SIGNALS:
    //render
    void contextCreated(int browserId, qint64 frameId);
private:
    QSharedPointer<QRemoteObjectDynamicReplica> reptr;// holds reference to replica
    bool m_binitConnection;
    QMap<QString, QSharedPointer<DynamicClient>> m_dynamicClientsMap;

    QMap<int, QSharedPointer<DynamicClient>> m_dynamicClientsIdMap;
 };

#endif

