#pragma once

#include "include/cef_values.h"
#include <QtCore>
#include "qcefvalue_qvar_convert.h"
namespace cefv8bind_protcool
{
	struct  CefMetaMethod
	{
		void pack(CefRefPtr<CefListValue> protocolMsg);
		bool unPack(const CefRefPtr<CefListValue> protocolMsg);

		QMetaMethod::MethodType methodType;
		int methodIndex;
		QString methodName;
		QStringList parameterTypes;
		QString	returnTypeName;
	};

	struct  CefMetaProperty
	{
		void pack(CefRefPtr<CefListValue> protocolMsg);
		bool unPack(const CefRefPtr<CefListValue> protocolMsg);

		int propertyIndex;
		QString propertyName;
		QString propertyTypeName;
		QVariant propertyValue;
		CefRefPtr<CefValue>	cef_propertyValue;
	};

	struct CefMetaObject
	{
		void pack(CefRefPtr<CefListValue> protocolMsg);
		bool unPack(const CefRefPtr<CefListValue> protocolMsg);

		QString					objectName;
		QString					className;
		int						objectId;
		int						parentId;
		QList<CefMetaMethod>	metaMethods;
		QList<CefMetaProperty>  metaProperties;
	};

	struct CefMetaType
	{
		CefRefPtr<CefValue> pack();
		bool unPack(const CefRefPtr<CefValue> protocolMsg);

		bool isValid(const CefRefPtr<CefValue> protocolMsg);

		QString typeName;
		CefRefPtr<CefValue>	value;

	private:
		QString flag;
	};
	struct PendingcallReq
	{
		CefString methodName;
		int	methodIndex;
		CefRefPtr<CefListValue> methodArgs;
		int objctId;
		qint64 callBackId;
		qint64 frameId;
		PendingcallReq(){} //signal need
		~PendingcallReq(){} // signal need
	};

	struct PendingcallResp
	{
		CefRefPtr<CefValue> returnValue;
		qint64 callBackId;
		qint64 frameId;
		bool invokeResult;
		PendingcallResp() {} //signal need
		~PendingcallResp() {} // signal need
	};
	Q_DECLARE_METATYPE(cefv8bind_protcool::CefMetaObject);
	Q_DECLARE_METATYPE(cefv8bind_protcool::CefMetaMethod);
	Q_DECLARE_METATYPE(cefv8bind_protcool::PendingcallReq);
}
