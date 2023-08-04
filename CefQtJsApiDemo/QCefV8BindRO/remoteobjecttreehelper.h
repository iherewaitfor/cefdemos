#ifndef REMOTEOBJECTTREEHELPER_H
#define REMOTEOBJECTTREEHELPER_H

#include <QtCore>
#include "remoteobjectstructs.h"
#include <QtRemoteObjects>

class RemoteObjectTreeHelper : public QObject
{
    Q_OBJECT
public:
    RemoteObjectTreeHelper(QObject*parent = nullptr);

    void addRootObject(QObject* rootObject);
    bool enableObjectsRemoting(QRemoteObjectHost * sourceNode);
    Q_INVOKABLE QVariantList getObjects();
    QList<RemoteObjectStruct> getObjectsList();
    QMap<QString, QObject*> getRemoteObjects();
private:
    void convertQObjectToRemoteStructList(QObject* itemObject, const QObject* parentObject, QList<RemoteObjectStruct>& objStructList);
    bool convertQObjectToRemoteStruct(QObject* itemObject, const QObject* parentObject, RemoteObjectStruct& remoteObjectStruct);

private:
    QList<QObject*> m_rootObjectList;
    QList<RemoteObjectStruct> m_objectsList;
    QMap<QString, QObject*> m_objectMap;
};

#endif
