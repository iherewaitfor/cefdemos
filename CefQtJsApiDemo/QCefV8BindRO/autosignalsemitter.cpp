#include "autosignalsemitter.h"
#include "qcefv8bindroapp.h"
#include "qcefv8bindroapp_p.h"
#include "qcefipcprotocol.h"
#include "qcefipcvalue.h"
#include "qcefv8objecthelper.h"

AutoSignalsEmitter::AutoSignalsEmitter(QMetaMethod sourceMethod,int objectId,QObject*parent)
    : QObject(parent), m_signalMetaMethod(sourceMethod),m_objectId(objectId){

}

AutoSignalsEmitter::~AutoSignalsEmitter()
{

}

void AutoSignalsEmitter::proxySlot()
{

}

void AutoSignalsEmitter::proxySignalEmit(void** _a)
{
    QVariantList args;
    int i = 0;
    foreach(QByteArray typeName, m_signalMetaMethod.parameterTypes())
    {
        int type = QMetaType::type(typeName.constData());

        // preincrement: start with 1
        QVariant arg(type, _a[++i]);
        // (_a[0] is return value)
        args << arg;
    }
    if (QCefV8BindAppRO::getInstance()->d_func()->getRenderDelegate()) {
        QObject* obj = sender();
        QString objName = obj->objectName();
        if (!QCefV8BindAppRO::getInstance()->d_func()->getReplicaTreeHelper()->getObjectsMap().contains(objName)) {
            return;
        }
        QSharedPointer<DynamicClient> dClient = QCefV8BindAppRO::getInstance()->d_func()->getReplicaTreeHelper()->getObjectsMap().value(objName);
        
        cefv8bind_protcool::DispatchReplicaSignaToJs msg;
        msg.objectId = dClient->property(KObjectId).toInt();
        
        //QString signature = m_signalMetaMethod.methodSignature();
        //QString name = m_signalMetaMethod.name();
        //signature;
        //name;
        msg.methodName = QCefValueConverter::to(QString(m_signalMetaMethod.name()));
        msg.methodIndex = m_signalMetaMethod.methodIndex();
        msg.methodArgs = QCefValueConverter::to(args)->GetList();
        QCefV8BindAppRO::getInstance()->d_func()->getRenderDelegate()->dispatchReplicaSignaToJs(msg);
    }
}