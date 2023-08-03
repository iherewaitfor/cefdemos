#include "qcefv8objecthelper.h"
#include "qcefv8bindroapp.h"
#include "qcefv8bindroapp_p.h"
#include "qcefv8bindutility.h"
#include "qcefvalue_qvar_convert.h"

using namespace cefv8bind_protcool;
const char KV8ObjectName[] = "V8ObjectName";
const char KObjectId[] = "KQObjectId";
const char KBrowserFrameId[] = "KBrowserFrameId";
const char KCefMetaObject[] = "CefMetaObject";
const char KCefMetaMethod[] = "CefMetaMethod";
const char KRenderV8Object[] = "RenderV8Object";
const char KParentName[] = "KParentName";

const char KObjectIdRO[] = "KObjectIdRO";
const char KBrowserFrameIdRO[] = "KBrowserFrameIdRO";
const char KCefMetaMethodIndexRO[] = "KCefMetaMethodIndexRO";

const char ConnectSignal[] = "connectSignal";
const char DisConnectSignal[] = "disconnect";
const char OnInitSdkOKNotify[] = "onInitSdkOKSignal"; //signal
const char InitSdkReadyFlag[] = "qcefSdkReadyFlag";

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

void QCefV8ObjectHelper::bindGlobalFunctions(CefRefPtr<CefV8Value> window, CefRefPtr<CefV8Handler> v8Handler)
{
	QStringList globalFuncs;
	globalFuncs.append(ConnectSignal);
	globalFuncs.append(DisConnectSignal);

	foreach(QString functionName, globalFuncs)
	{
		CefRefPtr<CefV8Value> v8Value = CefV8Value::CreateFunction(QCefValueConverter::to(functionName), v8Handler);
		const CefV8Value::PropertyAttribute attributes = static_cast<CefV8Value::PropertyAttribute>(V8_PROPERTY_ATTRIBUTE_READONLY | V8_PROPERTY_ATTRIBUTE_DONTENUM | V8_PROPERTY_ATTRIBUTE_DONTDELETE);
		window->SetValue(QCefValueConverter::to(functionName), v8Value, attributes);
	}

	QStringList	globalSignals;
	globalSignals << OnInitSdkOKNotify;
	foreach(QString functionName, globalSignals)
	{
		CefRefPtr<CefV8Value> v8Value = CefV8Value::CreateObject(nullptr, nullptr);

		QSharedPointer<QObject> obj = QSharedPointer<QObject>(new QObject());
		obj->setObjectName(functionName);
		obj->setProperty(KRenderV8Object, true);
		v8Value->SetUserData(new QCefV8ObjectHolder<QSharedPointer<QObject>>(obj));

		const CefV8Value::PropertyAttribute attributes = static_cast<CefV8Value::PropertyAttribute>(V8_PROPERTY_ATTRIBUTE_READONLY | V8_PROPERTY_ATTRIBUTE_DONTENUM | V8_PROPERTY_ATTRIBUTE_DONTDELETE);
		window->SetValue(QCefValueConverter::to(functionName), v8Value, attributes);
	}
}

CefRefPtr<CefV8Value> QCefV8ObjectHelper::bindV8Objects(const QList<cefv8bind_protcool::CefMetaObject>& cef_metaObjects, CefRefPtr<CefV8Context> context, CefRefPtr<CefV8Handler> v8Handler)
{
	CefRefPtr<CefV8Value> rootV8;
	foreach(CefMetaObject cefMetaObject, cef_metaObjects)
	{
		CefRefPtr<CefV8Value> v8Obj = createV8Object(cefMetaObject, v8Handler, context);

		if (cefMetaObject.parentId == 0)
		{
			rootV8 = v8Obj;
		}
	}

	return rootV8;
}

void QCefV8ObjectHelper::convertDynamicClientToCefObjects(QSharedPointer<DynamicClient> itemObject, QSharedPointer<DynamicClient> parentObject, QList<cefv8bind_protcool::CefMetaObject>& cef_metaObjects) {
	if (itemObject.isNull())
	{
		return;
	}
	foreach(QString key, QCefV8BindAppRO::getInstance()->d_func()->getReplicaTreeHelper()->getObjectsMap().keys()) {
		QSharedPointer<DynamicClient> object = QCefV8BindAppRO::getInstance()->d_func()->getReplicaTreeHelper()->getObjectsMap().value(key);
		QString parentName = object->property(KParentName).toString();
		QSharedPointer<DynamicClient> parentObj = QCefV8BindAppRO::getInstance()->d_func()->getReplicaTreeHelper()->getObjectsMap().value(parentName);
		CefMetaObject cef_metaObject;
		if (convertDynamicClientToCefObject(object, parentObj, cef_metaObject)) {
			cef_metaObjects.append(cef_metaObject);
		}
	}
}

bool QCefV8ObjectHelper::convertDynamicClientToCefObject(
	QSharedPointer<DynamicClient> itemObject,
	QSharedPointer<DynamicClient> parentObject,
	cefv8bind_protcool::CefMetaObject& cef_metaObject
)
{
	const QString objectName = itemObject->objectName();

	bool isNewValue = false;
	int uniqueId = 0;
	if (itemObject->property(KObjectId).toUInt() == 0)
	{
		isNewValue = true;
		uniqueId = QCefMetaUtilities::getSequenceId();
		itemObject->setProperty(KObjectId, uniqueId);
	}
	else
	{
		uniqueId = itemObject->property(KObjectId).toUInt();
	}

	const QMetaObject* metaObject = itemObject->getReplica()->metaObject();

	cef_metaObject.objectName = objectName;
	cef_metaObject.className = metaObject->className();
	cef_metaObject.objectId = uniqueId;
	cef_metaObject.parentId = parentObject ? parentObject->property(KObjectId).toUInt() : 0;

	QStringList methodFilter;
	methodFilter << "destroyed" << "initialized" << "objectNameChanged";
	for (int i = 0; i < metaObject->methodCount(); ++i)
	{
		QMetaMethod method = metaObject->method(i);
		if (methodFilter.contains(method.name())) {
			continue;
		}
		if (method.methodType() == QMetaMethod::Method || method.methodType() == QMetaMethod::Signal)
		{
			CefMetaMethod cef_metaMethod;
			cef_metaMethod.methodType = method.methodType();
			cef_metaMethod.methodIndex = i;
			cef_metaMethod.methodName = QCefMetaUtilities::getMethodName(QString::fromLatin1(method.methodSignature()));
			cef_metaMethod.returnTypeName = QString::fromLatin1(method.typeName());

			QList<QByteArray> types = method.parameterTypes();
			foreach(QByteArray item, types)
			{
				cef_metaMethod.parameterTypes << QString::fromLatin1(item.data(), item.size());
			}
			cef_metaObject.metaMethods.append(cef_metaMethod);
		}
		if (method.methodType() == QMetaMethod::Signal && isNewValue)
		{
			////to do: connect the signal of the object
			//AutoSignalsEmitter* autoEmitter = new AutoSignalsEmitter(method, const_cast<QObject*>(itemObject));
			//QObject::connect(itemObject, QString("2").append(method.methodSignature()).toStdString().c_str(),
			//	autoEmitter, SLOT(proxySlot()), Qt::DirectConnection);
			
			// to do connet when js to connect the signal. do not need connect yet.
		}
	}

	QStringList filterList;
	filterList << "node" << "state";
	//property
	for (int i = 0; i < metaObject->propertyCount(); ++i)
	{
		const QMetaProperty& metaProp = metaObject->property(i);
		if (metaProp.isUser() || !metaProp.isScriptable() || !metaProp.isReadable())
		{
			continue;
		}
		if (filterList.contains(metaProp.name())) {
			continue;
		}

		CefMetaProperty cef_metaProp;
		cef_metaProp.propertyIndex = metaProp.propertyIndex();
		cef_metaProp.propertyName = QString::fromLatin1(metaProp.name());
		cef_metaProp.propertyTypeName = QString::fromLatin1(metaProp.typeName());
		cef_metaProp.propertyValue = metaProp.read(itemObject->getReplica().data());
		cef_metaProp.cef_propertyValue = QCefValueConverter::to(cef_metaProp.propertyValue);

		cef_metaObject.metaProperties.append(cef_metaProp);
	}
	if (isNewValue)
	{
	//	QCefV8BindAppRO::getInstance()->d_func()->getObjectMgr()->insertBrowser(cef_metaObject.objectId, const_cast<QObject*>(itemObject));
	}
	return true;
}


bool QCefV8ObjectHelper::convertQObjectToCefObject(
	const QObject *itemObject, 
	const QObject *parentObject,
	cefv8bind_protcool::CefMetaObject &cef_metaObject
	)
{
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
		if (method.methodType() == QMetaMethod::Method || method.methodType() == QMetaMethod::Signal)
		{
			CefMetaMethod cef_metaMethod;
			cef_metaMethod.methodType = method.methodType();
			cef_metaMethod.methodIndex = i;
			cef_metaMethod.methodName = QCefMetaUtilities::getMethodName(QString::fromLatin1(method.methodSignature()));
			cef_metaMethod.returnTypeName = QString::fromLatin1(method.typeName());

			QList<QByteArray> types = method.parameterTypes();
			foreach(QByteArray item, types)
			{
				cef_metaMethod.parameterTypes << QString::fromLatin1(item.data(), item.size());
			}
			cef_metaObject.metaMethods.append(cef_metaMethod);
		}
		if (method.methodType() == QMetaMethod::Signal && isNewValue)
		{
			//to do: connect the signal of the object
			AutoSignalsEmitter* autoEmitter = new AutoSignalsEmitter(method, uniqueId, const_cast<QObject*>(itemObject));
			QObject::connect(itemObject, QString("2").append(method.methodSignature()).toStdString().c_str(),
				autoEmitter, SLOT(proxySlot()), Qt::DirectConnection);
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
	if (isNewValue)
	{
		QCefV8BindAppRO::getInstance()->d_func()->getObjectMgr()->insertBrowser(cef_metaObject.objectId, const_cast<QObject*>(itemObject));
	}
	return true;
}

CefRefPtr<CefV8Value> QCefV8ObjectHelper::getV8Object(quint32 objectId, CefRefPtr<CefV8Value> rootV8Object)
{
	if (objectId == 0)
	{
		return rootV8Object;
	}

	QStringList objectNames;
	getObjectPathName(objectId, objectNames);

	CefRefPtr<CefV8Value> parentV8Object = rootV8Object;
	for (int i = 0; i < objectNames.size(); ++i)
	{
		QString objectName = objectNames.at(i);
		CefRefPtr<CefV8Value> v8Object;
		if (!parentV8Object->HasValue(objectName.toStdString()))
		{
			//对象路径上没有对应的V8Object，就创建。
			v8Object = CefV8Value::CreateObject(nullptr, nullptr);
			const CefV8Value::PropertyAttribute attributes = static_cast<CefV8Value::PropertyAttribute>(V8_PROPERTY_ATTRIBUTE_READONLY | V8_PROPERTY_ATTRIBUTE_DONTENUM | V8_PROPERTY_ATTRIBUTE_DONTDELETE);
			parentV8Object->SetValue(objectName.toStdString(), v8Object, attributes);
		}
		else
		{
			v8Object = parentV8Object->GetValue(objectName.toStdString());
		}
		parentV8Object = v8Object;
	}

	return parentV8Object;
}

void QCefV8ObjectHelper::getObjectPathName(quint32 objectId, QStringList& objNames)
{
	QPointer<QCefObjectMgr> objectMgr = QCefV8BindAppRO::getInstance()->d_func()->getObjectMgr();

	QObject* metaObj = objectMgr->findRenderObject(objectId);
	if (metaObj == NULL)
	{
		return;
	}

	const cefv8bind_protcool::CefMetaObject metaObject = metaObj->property(KCefMetaObject).value<cefv8bind_protcool::CefMetaObject>();
	if (!metaObject.objectName.isEmpty())
	{
		objNames.push_front(metaObject.objectName);
	}

	if (metaObject.parentId == 0)
	{
		return;
	}
	else
	{
		getObjectPathName(metaObject.parentId, objNames);
	}
}

CefRefPtr<CefV8Value> QCefV8ObjectHelper::createV8Object(const cefv8bind_protcool::CefMetaObject& cefMetaObject, CefRefPtr<CefV8Handler> v8Handler, CefRefPtr<CefV8Context> context
)
{
	CefRefPtr<CefV8Value> rootV8Object = context->GetGlobal();
	CefRefPtr<CefV8Value> parentV8Object;
	if (!cefMetaObject.objectName.isEmpty())
	{
		parentV8Object = getV8Object(cefMetaObject.parentId, rootV8Object);
		if (parentV8Object == nullptr)
		{
			return nullptr;
		}
	}

	CefRefPtr<CefV8Value> v8Object = CefV8Value::CreateObject(nullptr, nullptr);
	// 把当前QObject接口的信息存储到一个QObject对象上，并把这个QObject对象存放到对应的V8Object的UserData
	QSharedPointer<QObject> obj = QSharedPointer<QObject>(new QObject());
	obj->setProperty(KCefMetaObject, QVariant::fromValue<cefv8bind_protcool::CefMetaObject>(cefMetaObject));
	obj->setProperty(KObjectId, cefMetaObject.objectId);
	obj->setProperty(KBrowserFrameId, context ? context->GetFrame()->GetIdentifier() : 0);
	v8Object->SetUserData(new QCefV8ObjectHolder<QSharedPointer<QObject>>(obj));

	//存当前对象
	QCefV8BindAppRO::getInstance()->d_func()->getObjectMgr()->insertRender(cefMetaObject.objectId, obj.data());

	foreach(CefMetaMethod metaMethod, cefMetaObject.metaMethods)
	{
		QString methodName = metaMethod.methodName;

		CefRefPtr<CefV8Value>  methodV8Object;
		if (metaMethod.methodType == QMetaMethod::Method)
		{
			methodV8Object = CefV8Value::CreateFunction(methodName.toStdString(), v8Handler);
		}
		else if (metaMethod.methodType == QMetaMethod::Signal)
		{
			methodV8Object = CefV8Value::CreateObject(nullptr, nullptr);
		}
		//存储方法的元数据，方便后续调用时获取。
		QSharedPointer<QObject> objMethod = QSharedPointer<QObject>(new QObject());
		objMethod->setProperty(KCefMetaMethod, QVariant::fromValue<cefv8bind_protcool::CefMetaMethod>(metaMethod));
		objMethod->setProperty(KObjectId, cefMetaObject.objectId);

		methodV8Object->SetUserData(new QCefV8ObjectHolder<QSharedPointer<QObject>>(objMethod));
		v8Object->SetValue(methodName.toStdString(), methodV8Object, V8_PROPERTY_ATTRIBUTE_NONE);
	}

	foreach(CefMetaProperty metaProperty, cefMetaObject.metaProperties)
	{
		QString propertyName = metaProperty.propertyName;
		CefRefPtr<CefValue> cef_value = metaProperty.cef_propertyValue;

		if (cef_value && cef_value->IsValid())
		{
			
			CefRefPtr<CefV8Value> propV8Object = QCefValueConverter::to(cef_value);
			if (propV8Object->IsValid())
			{
				v8Object->SetValue(QCefValueConverter::to(propertyName), propV8Object, V8_PROPERTY_ATTRIBUTE_READONLY);
			}
		}
	}
	if (parentV8Object)
	{
		//把创建的对象挂到parentObject上，如果有parent的话。
		const CefV8Value::PropertyAttribute attributes = static_cast<CefV8Value::PropertyAttribute>(V8_PROPERTY_ATTRIBUTE_READONLY | V8_PROPERTY_ATTRIBUTE_DONTENUM | V8_PROPERTY_ATTRIBUTE_DONTDELETE);
		parentV8Object->SetValue(cefMetaObject.objectName.toStdString(), v8Object, attributes);
	}

	return v8Object;
}

//////////////////////////////////////////////////////////////////////////
QRenderV8ObjectHelper::QRenderV8ObjectHelper()
{

}

QRenderV8ObjectHelper::~QRenderV8ObjectHelper()
{

}

void QRenderV8ObjectHelper::bindV8Object(CefRefPtr<CefV8Value> parentV8Object, QSharedPointer<QObject> itemObject, CefRefPtr<CefV8Handler> v8Handler)
{
	itemObject->setProperty(KV8ObjectName, itemObject->objectName());

	QCefV8ObjectHelper objectHelper;
	cefv8bind_protcool::CefMetaObject cef_metaObject;
	objectHelper.convertQObjectToCefObject(itemObject.data(), 0, cef_metaObject);

	createV8Object(cef_metaObject, parentV8Object, v8Handler, itemObject);
}

CefRefPtr<CefV8Value> QRenderV8ObjectHelper::createV8Object(const cefv8bind_protcool::CefMetaObject& cefMetaObject, CefRefPtr<CefV8Value> parentV8Object,
	CefRefPtr<CefV8Handler> v8Handler, QSharedPointer<QObject> itemObject)
{
	CefRefPtr<CefV8Value> v8Object = CefV8Value::CreateObject(nullptr, nullptr);


	itemObject->setProperty(KCefMetaObject, QVariant::fromValue<cefv8bind_protcool::CefMetaObject>(cefMetaObject));
	itemObject->setProperty(KRenderV8Object, true);
	v8Object->SetUserData(new QCefV8ObjectHolder<QSharedPointer<QObject>>(itemObject));

	QCefV8BindAppRO::getInstance()->d_func()->getObjectMgr()->insertRender(cefMetaObject.objectId, itemObject.data());

	foreach(CefMetaMethod metaMethod, cefMetaObject.metaMethods)
	{
		QString methodName = metaMethod.methodName;

		CefRefPtr<CefV8Value>  methodV8Object;
		if (metaMethod.methodType == QMetaMethod::Method)
		{
			methodV8Object = CefV8Value::CreateFunction(methodName.toStdString(), v8Handler);
		}
		else if (metaMethod.methodType == QMetaMethod::Signal)
		{
			methodV8Object = CefV8Value::CreateObject(nullptr, nullptr);
		}

		QSharedPointer<QObject> obj = QSharedPointer<QObject>(new QObject());
		obj->setProperty(KCefMetaMethod, QVariant::fromValue<cefv8bind_protcool::CefMetaMethod>(metaMethod));
		obj->setProperty(KObjectId, cefMetaObject.objectId);

		methodV8Object->SetUserData(new QCefV8ObjectHolder<QSharedPointer<QObject>>(obj));
		v8Object->SetValue(methodName.toStdString(), methodV8Object, V8_PROPERTY_ATTRIBUTE_NONE);
	}

	foreach(CefMetaProperty metaProp, cefMetaObject.metaProperties)
	{
		QString propName = metaProp.propertyName;
		CefRefPtr<CefValue> cef_value = metaProp.cef_propertyValue;

		if (cef_value && cef_value->IsValid())
		{
			CefRefPtr<CefV8Value> propV8Object = QCefValueConverter::to(cef_value);
			if (propV8Object->IsValid())
			{
				v8Object->SetValue(QCefValueConverter::to(propName), propV8Object, V8_PROPERTY_ATTRIBUTE_READONLY);
			}
		}
	}

	if (parentV8Object)
	{
		const CefV8Value::PropertyAttribute attributes = static_cast<CefV8Value::PropertyAttribute>(V8_PROPERTY_ATTRIBUTE_READONLY | V8_PROPERTY_ATTRIBUTE_DONTENUM | V8_PROPERTY_ATTRIBUTE_DONTDELETE);
		parentV8Object->SetValue(cefMetaObject.objectName.toStdString(), v8Object, attributes);
	}

	return v8Object;
}

