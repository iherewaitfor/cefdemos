#pragma once
#include "include/cef_v8.h"
#include "qcefipcvalue.h"

extern const char KObjectId[];
extern const char KV8ObjectName[];

class  QCefV8ObjectHelper
{
public:
	QCefV8ObjectHelper();
	virtual ~QCefV8ObjectHelper(void);

	//browser
	void convertQObjectToCefObjects(const QObject *rootObject, const QObject*parentObject, QList<cefv8bind_protcool::CefMetaObject> &cef_metaObjects);
	bool convertQObjectToCefObject(const QObject *itemObject, const QObject *parentObject, cefv8bind_protcool::CefMetaObject &cef_metaObject);
};