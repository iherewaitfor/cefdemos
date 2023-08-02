#pragma once
#include "include/cef_v8.h"
#include "qcefipcvalue.h"
#include "render/dynamicclient.h"

extern const char KObjectId[];
extern const char KV8ObjectName[];
extern const char KBrowserFrameId[];
extern const char KCefMetaObject[];
extern const char KCefMetaMethod[];
extern const char KRenderV8Object[];
extern const char KParentName[];

extern const char KObjectIdRO[];
extern const char KBrowserFrameIdRO[];
extern const char KCefMetaMethodIndexRO[];

extern const char ConnectSignal[];
extern const char DisConnectSignal[];
extern const char OnInitSdkOKNotify[];
extern const char InitSdkReadyFlag[];

class  QCefV8ObjectHelper
{
public:
	QCefV8ObjectHelper();
	virtual ~QCefV8ObjectHelper(void);

	//browser
	void convertQObjectToCefObjects(const QObject *rootObject, const QObject*parentObject, QList<cefv8bind_protcool::CefMetaObject> &cef_metaObjects);
	bool convertQObjectToCefObject(const QObject *itemObject, const QObject *parentObject, cefv8bind_protcool::CefMetaObject &cef_metaObject);
	//for render
	void convertDynamicClientToCefObjects(QSharedPointer<DynamicClient> dynamicClient, QSharedPointer<DynamicClient> parent,
		QList<cefv8bind_protcool::CefMetaObject>& cef_metaObjects);
	bool QCefV8ObjectHelper::convertDynamicClientToCefObject(
		QSharedPointer<DynamicClient> itemObject,
		QSharedPointer<DynamicClient> parentObject,
		cefv8bind_protcool::CefMetaObject& cef_metaObject
	);

	CefRefPtr<CefV8Value> bindV8Objects(const QList<cefv8bind_protcool::CefMetaObject>& cef_metaObjects,
		CefRefPtr<CefV8Context> context, CefRefPtr<CefV8Handler> v8Handler);

	void bindGlobalFunctions(CefRefPtr<CefV8Value> window, CefRefPtr<CefV8Handler> v8Handler);

	CefRefPtr<CefV8Value> createV8Object(const cefv8bind_protcool::CefMetaObject& cefMetaObject, CefRefPtr<CefV8Handler> v8Handler, CefRefPtr<CefV8Context> context);

private:
	CefRefPtr<CefV8Value> getV8Object(quint32 objectId, CefRefPtr<CefV8Value> rootV8Object);
	void getObjectPathName(quint32 objectId, QStringList& objNames);
};

class QRenderV8ObjectHelper
{
public:
	QRenderV8ObjectHelper();
	virtual ~QRenderV8ObjectHelper();

	void bindV8Object(CefRefPtr<CefV8Value> parentV8Object, QSharedPointer<QObject> itemObject, CefRefPtr<CefV8Handler> v8Handler);

private:
	CefRefPtr<CefV8Value> createV8Object(const cefv8bind_protcool::CefMetaObject& cefMetaObject, CefRefPtr<CefV8Value> parentV8Object,
		CefRefPtr<CefV8Handler> v8Handler, QSharedPointer<QObject> itemObject);
};