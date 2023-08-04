#include "remoteobjecttreehelper.h"

RemoteObjectTreeHelper::RemoteObjectTreeHelper(QObject*parent)
    : QObject(parent){
    setObjectName("RemoteObjectTreeHelper");
}

void RemoteObjectTreeHelper::addRootObject(QObject* rootObject) {
	if (m_rootObjectList.contains(rootObject)) {
		return;
	}
	m_rootObjectList.append(rootObject);

//	convertQObjectToRemoteStructList(m_rootObject, nullptr, m_objectsList);
}

bool RemoteObjectTreeHelper::enableObjectsRemoting(QRemoteObjectHost* sourceNode) {
	if (!sourceNode) {
		return false;
	}
	// enable remoting of source object
	// enableRemoting the remote object tree
	Q_FOREACH(const QString key, m_objectMap.keys()) {
		QObject* remoteObj = (QObject*)m_objectMap.value(key);
		if (remoteObj) {
			sourceNode->enableRemoting(remoteObj, key);
		}
	}
	//remoting the tree helper
	sourceNode->enableRemoting(this, "QCefRemoteObjectTreeHelper");
    return true;
}

QVariantList RemoteObjectTreeHelper::getObjects() {
	QVariantList list;
	foreach(RemoteObjectStruct objStruct, m_objectsList) {
		QVariantList temObj;
		temObj.append(objStruct.objName);
		temObj.append(objStruct.parentName);
		temObj.append(objStruct.v8Name);
		list.push_back(temObj);
	}
    return list;
}

QList<RemoteObjectStruct> RemoteObjectTreeHelper::getObjectsList() {
	m_objectsList.clear();
	foreach(QObject * rootObject, m_rootObjectList) {
		convertQObjectToRemoteStructList(rootObject, nullptr, m_objectsList);
	}
	return m_objectsList;
}

QMap<QString, QObject*> RemoteObjectTreeHelper::getRemoteObjects() {
	return m_objectMap;
}


void RemoteObjectTreeHelper::convertQObjectToRemoteStructList(QObject* itemObject, const QObject* parentObject, QList<RemoteObjectStruct>& objStructList) {
	if (!itemObject)
	{
		return;
	}
	RemoteObjectStruct remoteObjectStruct;
	if (convertQObjectToRemoteStruct(itemObject, parentObject, remoteObjectStruct))
	{
		objStructList.append(remoteObjectStruct);
	}

	QObjectList lists;
	QList<QByteArray> propNames = itemObject->dynamicPropertyNames();
	foreach(QByteArray propName, propNames)
	{
		const QVariant var = itemObject->property(propName);
		if (var.type() == QMetaType::QObjectStar && !var.isNull())
		{
			QObject* obj = var.value<QObject*>();
			lists << obj;
		}
	}

	for (int i = 0; i < lists.count(); ++i)
	{
		convertQObjectToRemoteStructList(lists.at(i), itemObject, objStructList);
	}
}
bool RemoteObjectTreeHelper::convertQObjectToRemoteStruct(QObject* itemObject, const QObject* parentObject, RemoteObjectStruct& remoteObjectStruct) {
	m_objectMap.insert(itemObject->objectName(), itemObject);

	const QString objectName = itemObject->objectName();
	QString parentName = "";
	QString v8Name = "";
	if (itemObject->parent()) {
		parentName = itemObject->parent()->objectName();
	}
	QVariant varV8Name = itemObject->property("v8Name");
	if (varV8Name.isValid()) {
		v8Name = varV8Name.toString();
	}
	remoteObjectStruct.objName = objectName;
	remoteObjectStruct.parentName = parentName;
	remoteObjectStruct.v8Name = v8Name;
	return true;
}
