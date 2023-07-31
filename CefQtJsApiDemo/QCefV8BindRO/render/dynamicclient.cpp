#include "dynamicclient.h"
 #include <QMetaMethod>
#include <QRemoteObjectPendingCall>

// constructor
DynamicClient::DynamicClient(QSharedPointer<QRemoteObjectDynamicReplica> ptr) :
    QObject(nullptr), clientSwitchState(false), reptr(ptr), m_binitConnection(false)
{
    //connect signal for replica valid changed with signal slot initialization
    QObject::connect(reptr.data(), SIGNAL(initialized()), this, SLOT(initConnection_slot()), Qt::UniqueConnection);
    QObject::connect(reptr.data(), SIGNAL(stateChanged(QRemoteObjectReplica::State, QRemoteObjectReplica::State)), 
        this, SLOT(stateChanged_slot(QRemoteObjectReplica::State, QRemoteObjectReplica::State)));
}

//destructor
DynamicClient::~DynamicClient()
{

}

// Function to initialize connections between slots and signals
void DynamicClient::initConnection_slot()
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
    
    //replica 不同步对象树的信息。所以以下信息为空。
    for (int i = 0; i < reptr->children().size(); i++) {
        QObject* o = reptr->children().at(i);
        qDebug() << " child   " << i << " objectName " <<
            o->objectName();
    }
    qDebug() << " objcetName: " << reptr->objectName();

    // connect source replica signal counterChanged()
   QObject::connect(reptr.data(), SIGNAL(counterChanged(int)), this, SLOT(onCounterChanged_slot(int)));
   // connect client's echoSwitchState(..) signal with replica's server_slot(..) to echo back received state
   QObject::connect(this, SIGNAL(makeSourceEmit()),reptr.data(), SLOT(onIncreace()));

    QObject::connect(&testTimer, SIGNAL(timeout()), this, SLOT(timerOut()));
    testTimer.setInterval(5000);
//    testTimer.start();
   QRemoteObjectPendingCall call;
   bool success = QMetaObject::invokeMethod(reptr.data(), "addCount", Qt::AutoConnection,
	   Q_RETURN_ARG(QRemoteObjectPendingCall, call),
       Q_ARG(int, 5)
   );
   success;
   //to do:  how to free?   temply free at slot.
   QRemoteObjectPendingCallWatcher* callwathcer = new QRemoteObjectPendingCallWatcher(call);
   QObject::connect(callwathcer, SIGNAL(finished(QRemoteObjectPendingCallWatcher*)), this, SLOT(pendingCallResult(QRemoteObjectPendingCallWatcher*)));

}

//monitor the replica disconnect with the source
void DynamicClient::stateChanged_slot(QRemoteObjectReplica::State state, QRemoteObjectReplica::State oldState){
    
    if (QRemoteObjectReplica::State::Suspect == state && QRemoteObjectReplica::State::Valid == oldState) {
        // source断开了
        // to do 
    }
    else if (QRemoteObjectReplica::State::Valid == state && QRemoteObjectReplica::State::Suspect == oldState) {
        //重新连接上了
        // to do 
    }
    qDebug() << "Replica stateChanged_slot state= " << state << " oldState: oldState=" << oldState;
}

void DynamicClient::onCounterChanged_slot(int counter)
{
   int c = reptr->property("counter").toInt();
   qDebug() << "Replica property counter= " << c << " onCounterChanged_slot: counter=" << counter;

}

void DynamicClient::pendingCallResult(QRemoteObjectPendingCallWatcher* call) {
    //异步回调结果
    qDebug() << reptr->objectName() << "pendingCallResult call add : " << QMetaType::typeName(call->returnValue().type()) << call->returnValue().toString();
    sender()->deleteLater(); // 待优化。若无信号返回，则会造成内存泄漏
}
void DynamicClient::timerOut() {
    //带返回值，但不需要返回值时
    QMetaObject::invokeMethod(reptr.data(), "addQString", Qt::AutoConnection,
        Q_ARG(QString, "500")
    );
    Q_EMIT makeSourceEmit();
    int c = reptr->property("counter").toInt();
    qDebug() << "timerOut Replica property counter= " << c;
}


QSharedPointer<QRemoteObjectDynamicReplica> DynamicClient::getReplica() {
    return reptr;
}


void DynamicClient::callRepalicaMethod(QString replicaID, QString Method, QVariantList arguments) {

}