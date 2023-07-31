#include "autosignalsemitter.h"
#include "qcefv8bindroapp.h"
#include "qcefv8bindroapp_p.h"
#include "qcefipcprotocol.h"
#include "qcefipcvalue.h"
#include "qcefv8objecthelper.h"

AutoSignalsEmitter::AutoSignalsEmitter(QMetaMethod sourceMethod,QObject*parent)
    : QObject(parent), m_signalMetaMethod(sourceMethod){

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
    if (QCefV8BindAppRO::getInstance()->d_func()->getBrowserDelegate()) {
        QObject* obj = sender();
        cefv8bind_protcool::EmitSignalMsg msg;
        msg.objectId= obj->property(KObjectId).toUInt();
        //QString signature = m_signalMetaMethod.methodSignature();
        //QString name = m_signalMetaMethod.name();
        //signature;
        //name;
        msg.methodName = QCefValueConverter::to(QString(m_signalMetaMethod.name()));
        msg.methodIndex = m_signalMetaMethod.methodIndex();
        msg.methodArgs = QCefValueConverter::to(args)->GetList();
//       QCefV8BindAppRO::getInstance()->d_func()->getBrowserDelegate()->sendProcessMessage(PID_RENDERER, msg.makeIPCMessage());
    }
    
    //to do : send siganle msg to render process.
    
//    emit proxySignal(sender(), m_signalMetaMethod, args);
}