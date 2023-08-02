#include "metainvoker.h"
#include <QtCore>
#include <QtRemoteObjects>
#include "qcefv8bindroapp.h"
#include "qcefv8bindroapp_p.h"

bool MetaInvoker::run() {
	QList<QGenericArgument> tempArgs;
	for (int i = 0; i < args.size(); i++) {
        QVariant& arg = args[i];
        QGenericArgument gArg(QMetaType::typeName(arg.userType()),
            const_cast<void*>(arg.constData())
        );
        tempArgs << gArg;
	}
    if (metaMethod.returnType() != QMetaType::Void) {
        QRemoteObjectPendingCall call;
        ok = metaMethod.invoke(
            object,
            Qt::AutoConnection,
            Q_RETURN_ARG(QRemoteObjectPendingCall, call),
            tempArgs.value(0),
            tempArgs.value(1),
            tempArgs.value(2),
            tempArgs.value(3),
            tempArgs.value(4),
            tempArgs.value(5),
            tempArgs.value(6),
            tempArgs.value(7),
            tempArgs.value(8),
            tempArgs.value(9)
        );
        QRemoteObjectPendingCallWatcher* callwathcer = new QRemoteObjectPendingCallWatcher(call);
        callwathcer->setProperty("callbackId", callbackId);
        callwathcer->setProperty("frameId", frameId);
        QObject::connect(callwathcer, SIGNAL(finished(QRemoteObjectPendingCallWatcher*)), 
            QCefV8BindAppRO::getInstance()->d_func(), SLOT(pendingCallResult(QRemoteObjectPendingCallWatcher*)));
        //to do . can save pointer callwathcer, and free at some time later (ex:120 secondes),
        // if the finished signal not emited.(the source remote object perhaps has crashed.)
    }
    else {
     ok = metaMethod.invoke(
            object,
            Qt::AutoConnection,
            tempArgs.value(0),
            tempArgs.value(1),
            tempArgs.value(2),
            tempArgs.value(3),
            tempArgs.value(4),
            tempArgs.value(5),
            tempArgs.value(6),
            tempArgs.value(7),
            tempArgs.value(8),
            tempArgs.value(9)
        );
    }
    return ok;
}