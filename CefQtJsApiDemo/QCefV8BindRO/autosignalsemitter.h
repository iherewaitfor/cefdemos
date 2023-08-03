#ifndef AUTOSIGNALSEMITTER_H
#define AUTOSIGNALSEMITTER_H

#include <QObject>
#include <QMetaMethod>

class AutoSignalsEmitter : public QObject
{
    Q_OBJECT_FAKE

public:
    explicit AutoSignalsEmitter(QMetaMethod sourceMethod, int objectId, QObject* parent = nullptr);
    ~AutoSignalsEmitter();
private slots:
    //占位槽，用于连接源对象的像信号处理。
    void proxySlot();
signals:
    void proxySignal(QObject* sender, QMetaMethod signal, QVariantList arguments);
private:
    //实际的信号转发
    void proxySignalEmit(void** _a);
private:
    QMetaMethod m_signalMetaMethod;
    int m_objectId;
};

#endif
