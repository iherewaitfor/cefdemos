#ifndef METAINVOKER_H
#define METAINVOKER_H

#include <QtCore>

struct MetaInvoker
{
    QObject* object;
    QMetaMethod metaMethod;
    QVariantList args;
    //���к��ȡ
    bool ok = false;
    QVariant result;
    qint64 callbackId;
    qint64 frameId;
    bool MetaInvoker::run();
};

#endif
