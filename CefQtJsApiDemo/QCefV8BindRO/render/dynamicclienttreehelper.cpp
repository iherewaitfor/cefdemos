#include "dynamicclienttreehelper.h"
#include <QMetaMethod>
#include <QRemoteObjectPendingCall>
#include "../qcefv8objecthelper.h"
#include "../qcefipcvalue.h"

int getSequenceId()
{
    static volatile long s_id = 0;
    return InterlockedIncrement(&s_id);
}
// constructor
DynamicClientTreeHelper::DynamicClientTreeHelper(QSharedPointer<QRemoteObjectDynamicReplica> ptr) :
    QObject(nullptr), reptr(ptr), m_binitConnection(false)
{
    //connect signal for replica valid changed with signal slot initialization
   QObject::connect(reptr.data(), SIGNAL(initialized()), this, SLOT(initConnection_slot()));
    QObject::connect(reptr.data(), SIGNAL(stateChanged(QRemoteObjectReplica::State, QRemoteObjectReplica::State)), 
        this, SLOT(stateChanged_slot(QRemoteObjectReplica::State, QRemoteObjectReplica::State)));
}

//destructor
DynamicClientTreeHelper::~DynamicClientTreeHelper()
{

}

const QMap<QString, QSharedPointer<DynamicClient>>& DynamicClientTreeHelper::getObjectsMap() {
    return m_dynamicClientsMap;
}
const QMap<int, QSharedPointer<DynamicClient>>& DynamicClientTreeHelper::getObjectsIdMap() {
    return m_dynamicClientsIdMap;
}
// Function to initialize connections between slots and signals
void DynamicClientTreeHelper::initConnection_slot()
{
    if (m_binitConnection) {
        return;
    }
    m_binitConnection = true;

    QRemoteObjectPendingCall call;
    bool success = QMetaObject::invokeMethod(reptr.data(), "getObjects", Qt::AutoConnection,
        Q_RETURN_ARG(QRemoteObjectPendingCall, call)
    );
    ;
    if (success) {
        //to do:  how to free?   temply free at slot. 
        QRemoteObjectPendingCallWatcher* callwathcer = new QRemoteObjectPendingCallWatcher(call);
        QObject::connect(callwathcer, SIGNAL(finished(QRemoteObjectPendingCallWatcher*)), this, SLOT(pendingCallResult(QRemoteObjectPendingCallWatcher*)));
    }


}

//monitor the replica disconnect with the source
void DynamicClientTreeHelper::stateChanged_slot(QRemoteObjectReplica::State state, QRemoteObjectReplica::State oldState){
    if (QRemoteObjectReplica::State::Suspect == state && QRemoteObjectReplica::State::Valid == oldState) {
        // source断开了
        // to do 
    }
    else if (QRemoteObjectReplica::State::Valid == state && QRemoteObjectReplica::State::Suspect == oldState) {
        //重新连接上了
        // to do 
    }
}

void DynamicClientTreeHelper::pendingCallResult(QRemoteObjectPendingCallWatcher* call) {

    //异步回调结果
    QVariantList reobjList = call->returnValue().toList();
    foreach(QVariant obj, reobjList) {
        QVariantList list = obj.toList();
        if (list.size() < 3) {
            continue;
        }
        QString objectName = list.at(0).toString();
        QString parentName = list.at(1).toString();
        QString v8Name = list.at(2).toString();
        if (!m_dynamicClientsMap.contains(objectName)) {
            QSharedPointer<QRemoteObjectDynamicReplica> pRep;
            pRep.reset(reptr->node()->acquireDynamic(objectName)); // acquire replica of source from host node

            pRep->setObjectName(objectName);
            QSharedPointer<DynamicClient> pClient;
            pClient.reset(new DynamicClient(pRep));
            int objectId = getSequenceId();
            pClient->setObjectName(objectName);
            pClient->setProperty(KObjectId, objectId);
            pClient->setProperty(KParentName, parentName);
            if (v8Name.isEmpty()) {
                v8Name = objectName;
            }
            pClient->setProperty("v8Name", v8Name);
            
            m_dynamicClientsMap[objectName] = pClient;
            m_dynamicClientsIdMap[objectId] = pClient;
        }
    }
    call->deleteLater();// 待优化。若无信号返回，则会造成内存泄漏。可new时存下来，定时删除缓存，及删掉watcher.
}

void DynamicClientTreeHelper::getMetaObjects(QList<cefv8bind_protcool::CefMetaObject>& cef_metaObjects) {
    QCefV8ObjectHelper objectHelper;
    objectHelper.convertDynamicClientToCefObjects(cef_metaObjects);
}

//render
void DynamicClientTreeHelper::contextCreated_slot(int browserId, qint64 frameId) {
    // get the objectsTree and send to bind.
    QList<cefv8bind_protcool::CefMetaObject> cef_metaObjects;
    getMetaObjects(cef_metaObjects);
    QCefV8BindAppRO::getInstance()->d_func()->getRenderDelegate()->tobindV8Objects(cef_metaObjects, frameId);
}