#include "dynamicclienttreehelper.h"
#include <QMetaMethod>
#include <QRemoteObjectPendingCall>

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

const QMap<QString, QSharedPointer<QRemoteObjectDynamicReplica>>& DynamicClientTreeHelper::getObjectsMap() {
    return m_objectsMap;
}
// Function to initialize connections between slots and signals
void DynamicClientTreeHelper::initConnection_slot()
{
    if (m_binitConnection) {
        return;
    }
    m_binitConnection = true;
    const QMetaObject* meta = reptr->metaObject();
    for (int i = 0; i < meta->methodCount(); i++) {
        QMetaMethod method = meta->method(i);
        //MethodType:
        //0 plain
        //1 signal
        //2 slot
        //3 constructor
        qDebug() << " method " << i<< " MethodType" << method.methodType() << " " <<
            method.returnType()<< QMetaType::typeName(method.returnType()) << " " << method.methodSignature();
    }
    qDebug() << " propertyCount: " << meta->propertyCount();
    for (int i = 0; i < meta->propertyCount(); i++) {
        QMetaProperty property = meta->property(i);
        qDebug() << " property " << i << " " <<
            property.name() << " " << property.typeName();
    }
    qDebug() << " propertyCount: " << meta->propertyCount();
    
    //replica ��ͬ������������Ϣ������������ϢΪ�ա�
    for (int i = 0; i < reptr->children().size(); i++) {
        QObject* o = reptr->children().at(i);
        qDebug() << " child   " << i << " objectName " <<
            o->objectName();
    }
    qDebug() << " objcetName: " << reptr->objectName();
    

    QRemoteObjectPendingCall call;
    bool success = QMetaObject::invokeMethod(reptr.data(), "getObjects", Qt::AutoConnection,
        Q_RETURN_ARG(QRemoteObjectPendingCall, call)
    );
    success;
    //to do:  how to free?   temply free at slot.
    QRemoteObjectPendingCallWatcher* callwathcer = new QRemoteObjectPendingCallWatcher(call);
    QObject::connect(callwathcer, SIGNAL(finished(QRemoteObjectPendingCallWatcher*)), this, SLOT(pendingCallResult(QRemoteObjectPendingCallWatcher*)));


}

//monitor the replica disconnect with the source
void DynamicClientTreeHelper::stateChanged_slot(QRemoteObjectReplica::State state, QRemoteObjectReplica::State oldState){
    if (QRemoteObjectReplica::State::Suspect == state && QRemoteObjectReplica::State::Valid == oldState) {
        // source�Ͽ���
        // to do 
    }
    else if (QRemoteObjectReplica::State::Valid == state && QRemoteObjectReplica::State::Suspect == oldState) {
        //������������
        // to do 
    }
    qDebug() << "Replica stateChanged_slot state= " << state << " oldState: oldState=" << oldState;
}

void DynamicClientTreeHelper::pendingCallResult(QRemoteObjectPendingCallWatcher* call) {

    //�첽�ص����
    qDebug() << "pendingCallResult call getObjects size : " << QMetaType::typeName(call->returnValue().type()) << call->returnValue().toList().size();
    sender()->deleteLater(); // ���Ż��������źŷ��أ��������ڴ�й©
    QVariantList reobjList = call->returnValue().toList();
    foreach(QVariant obj, reobjList) {
        QVariantList list = obj.toList();
        if (list.size() < 2) {
            continue;
        }
        QString objectName = list.at(0).toString();
        QString parentName = list.at(1).toString();
        if (!m_objectsMap.contains(objectName)) {
            QSharedPointer<QRemoteObjectDynamicReplica> pRep;
            pRep.reset(reptr->node()->acquireDynamic(objectName)); // acquire replica of source from host node
            pRep->setObjectName(objectName);
            //m_objectsMap.insert(objectName, pRep);
            m_objectsMap[objectName] = pRep;
            QSharedPointer<DynamicClient> pClient;
            pClient.reset(new DynamicClient(pRep));
            m_dynamicClientsMap[objectName] = pClient;
        }
    }
}