#pragma once
#include <QtCore>
#include "include/cef_values.h"
#include "include/cef_v8.h"

class QCefValueConverter
{
public:
	static CefString to(const QString & string);
	static QString from(const CefString & string);
	template <typename T>
	static CefRefPtr<CefValue> to(T const & v);
	static CefRefPtr<CefValue> QCefValueConverter::to(const QVariant& var);
	static CefRefPtr<CefValue> QCefValueConverter::to(const QStringList& v);
	static CefRefPtr<CefValue> QCefValueConverter::to(const QVariantList& v);
	static CefRefPtr<CefValue> QCefValueConverter::to(const QVariantMap& v);
	template<>
	static CefRefPtr<CefValue> to<QVariantMap>(QVariantMap const& v);

	static QVariant from(const CefRefPtr<CefValue>& v);

	static QVariantList convertFromCefListToVariantList(const  CefRefPtr<CefValue> & arg);
	static QVariant converCefValueToVariant(const CefRefPtr<CefValue> arg);
	static QVariantMap convertFromCefValueToVariantMap(CefRefPtr<CefValue>& arg);
};
//Q_DECLARE_METATYPE(CefRefPtr<CefValue>);



//class CefValue_CefV8_Converter
//{
//public:
//	static CefRefPtr<CefV8Value> to( CefRefPtr<CefValue> const &v, const char *typeName );
//	static CefRefPtr<CefValue> from( CefRefPtr<CefV8Value> const & v);
//};
