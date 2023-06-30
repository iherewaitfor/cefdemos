#include "metainvoker.h"

bool MetaInvoker::run() {
	QList<QGenericArgument> tempArgs;
	for (int i = 0; i < args.size(); i++) {
        QVariant& arg = args[i];
        QGenericArgument gArg(QMetaType::typeName(arg.userType()),
            const_cast<void*>(arg.constData())
        );
        tempArgs << gArg;
	}
    const int retTypeId = metaMethod.returnType();
    void* retVal = QMetaType::create(retTypeId);
    QGenericReturnArgument returnArg( QMetaType::typeName(retTypeId), retVal);
    ok = metaMethod.invoke(
        object,
        Qt::DirectConnection,
        returnArg,
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
    result = QVariant(retTypeId, retVal);
    return ok;
}