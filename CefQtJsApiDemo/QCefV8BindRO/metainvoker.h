#ifndef METAINVOKER_H
#define METAINVOKER_H

#include <QtCore>

struct MetaInvoker
{
    QObject* object;
    QMetaMethod metaMethod;
    QVariantList args;
    //运行后读取
    bool ok = false;
    QVariant result;
    qint64 callbackId;
    bool MetaInvoker::run();
};

#endif
