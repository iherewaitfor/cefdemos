#include "qcefv8objecthelper.h"

using namespace cefv8bind_protcool;
const char KV8ObjectName[] = "V8ObjectName";
const char KObjectId[] = "KQObjectId";

namespace QCefMetaUtilities {
	int getSequenceId()
	{
		static volatile long s_id = 0;
		return InterlockedIncrement(&s_id);
	}
	QString getMethodName(const QString& signature)
	{
		QString methodName;
		int findIndex = signature.lastIndexOf("(");
		if (findIndex == -1)
		{
			methodName = signature;
		}
		else
		{
			methodName = signature.left(findIndex);
		}
		return methodName;
	}
};

QCefV8ObjectHelper::QCefV8ObjectHelper()
{
}

QCefV8ObjectHelper::~QCefV8ObjectHelper(void)
{

}


void QCefV8ObjectHelper::convertQObjectToCefObjects(const QObject *itemObject, const QObject*parentObject, QList<cefv8bind_protcool::CefMetaObject> &cef_metaObjects)
{
    if (!itemObject)
    {
        return;
    }
	CefMetaObject cef_metaObject;
	if (convertQObjectToCefObject(itemObject, parentObject, cef_metaObject))
	{
		cef_metaObjects.append(cef_metaObject);
	}

	QObjectList lists;
	QList<QByteArray> propNames = itemObject->dynamicPropertyNames();
	foreach(QByteArray propName, propNames)
	{
		const QVariant var = itemObject->property(propName);
		if (var.type() == QMetaType::QObjectStar && !var.isNull())
		{
			QObject*obj = var.value<QObject*>();
			//obj->setProperty(KV8ObjectName, QString::fromLatin1(propName));
			lists << obj;
		}
	}

	for (int i = 0; i < lists.count(); ++i)
	{
		convertQObjectToCefObjects(lists.at(i), itemObject, cef_metaObjects);
	}
}

bool QCefV8ObjectHelper::convertQObjectToCefObject(
	const QObject *itemObject, 
	const QObject *parentObject,
	cefv8bind_protcool::CefMetaObject &cef_metaObject
	)
{
	//const QString objectName = itemObject->property(KV8ObjectName).toString();
	const QString objectName = itemObject->objectName();

	bool isNewValue = false;
	int uniqueId = 0;
	if (itemObject->property(KObjectId).toUInt() == 0)
	{
		isNewValue = true;
		uniqueId = QCefMetaUtilities::getSequenceId();
		const_cast<QObject*>(itemObject)->setProperty(KObjectId, uniqueId);
	}
	else
	{
		uniqueId = itemObject->property(KObjectId).toUInt();
	}

	const QMetaObject* metaObject = itemObject->metaObject();

	cef_metaObject.objectName = objectName;
	cef_metaObject.className = metaObject->className();
	cef_metaObject.objectId = uniqueId;
	cef_metaObject.parentId = parentObject ? parentObject->property(KObjectId).toUInt() : 0;

	QStringList methods;
	for(int i = 0; i < metaObject->methodCount(); ++i)
	{
		QMetaMethod method = metaObject->method(i);
		if (method.methodType() == QMetaMethod::Method)
		{
			CefMetaMethod cef_metaMethord;
			cef_metaMethord.methodType = method.methodType();
			cef_metaMethord.methodIndex = i;
			cef_metaMethord.methodName = QCefMetaUtilities::getMethodName(QString::fromLatin1(method.methodSignature()));
			cef_metaMethord.returnTypeName = QString::fromLatin1(method.typeName());

			QList<QByteArray> types = method.parameterTypes();
			foreach(QByteArray item, types)
			{
				cef_metaMethord.parameterTypes << QString::fromLatin1(item.data(), item.size());
			}

			cef_metaObject.metaMethods.append(cef_metaMethord);
		}
	}

	//property
	for(int i = 0; i < metaObject->propertyCount(); ++i)
	{
		const QMetaProperty &metaProp =  metaObject->property(i);
		if (metaProp.isUser() || !metaProp.isScriptable() || !metaProp.isReadable())
		{
			continue;
		}

		CefMetaProperty cef_metaProp;
		cef_metaProp.propertyIndex = metaProp.propertyIndex();
		cef_metaProp.propertyName = QString::fromLatin1(metaProp.name());
		cef_metaProp.propertyTypeName = QString::fromLatin1(metaProp.typeName());
		cef_metaProp.propertyValue = metaProp.read(itemObject);

		cef_metaObject.metaProperties.append(cef_metaProp);
	}
	return true;
}

