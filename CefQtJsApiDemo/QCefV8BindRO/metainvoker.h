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
    bool MetaInvoker::run();
};

#endif
