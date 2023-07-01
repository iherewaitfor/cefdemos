#include "autosignalsemitter.h"
#include "qcefv8bindapp.h"

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
    //to do : send siganle msg to render process.
    
//    emit proxySignal(sender(), m_signalMetaMethod, args);
}