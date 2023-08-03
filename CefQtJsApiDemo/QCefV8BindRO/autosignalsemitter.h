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
    //ռλ�ۣ���������Դ��������źŴ���
    void proxySlot();
signals:
    void proxySignal(QObject* sender, QMetaMethod signal, QVariantList arguments);
private:
    //ʵ�ʵ��ź�ת��
    void proxySignalEmit(void** _a);
private:
    QMetaMethod m_signalMetaMethod;
    int m_objectId;
};

#endif
