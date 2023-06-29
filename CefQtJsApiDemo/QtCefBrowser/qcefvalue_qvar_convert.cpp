﻿#include "qcefipcvalue.h"
#include "qcefipcprotocol.h"
#include "qcefvalue_qvar_convert.h"

#include <QtCore/QTypeInfo>
#include <QtCore>

CefString QCefValueConverter::to(const QString& string)
{
	return !string.isEmpty() ? CefString((const char16*)string.utf16(), string.length(), true) : CefString();
}
QString QCefValueConverter::from(const CefString& string)
{
	return !string.empty() ? QString::fromUtf16((const ushort*)string.c_str(), string.length()) : QString();
}
template <typename T>
CefRefPtr<CefValue> QCefValueConverter::to(T const& v)
{
	QVariant var = QVariant(v);
	CefRefPtr<CefValue> retValue = CefValue::Create();
	retValue = QCefValueConverter::to(v);
	return retValue;
}

CefRefPtr<CefValue> QCefValueConverter::to( const QStringList& v) {
	CefRefPtr<CefValue> retValue = CefValue::Create();
	CefRefPtr<CefListValue> list = CefListValue::Create();
	for (int i = 0; i < v.length(); i++)
	{
		list->SetString(i, QCefValueConverter::to(v.at(i)));
	}
	return retValue;
}

CefRefPtr<CefValue> QCefValueConverter::to( const QVariantList& v) {
	CefRefPtr<CefValue> retValue = CefValue::Create();
	CefRefPtr<CefListValue> list = CefListValue::Create();
	for (int i = 0; i < v.length(); i++) {
		//CefRefPtr<CefValue> QCefValueConverter::to(const QVariant & var)
		list->SetValue(i, QCefValueConverter::to(v.at(i)));
	}
	retValue->SetList(list);
	return retValue;
}

CefRefPtr<CefValue> QCefValueConverter::to( const QVariant& var) {
	CefRefPtr<CefValue> retValue = CefValue::Create();
	switch (var.userType()) {
		case QVariant::Bool: {
			retValue->SetBool(var.toBool());
		}
		break;
		case QVariant::Int: {
			retValue->SetInt(var.toInt());
		}
		break;
		case QVariant::Double: {
			retValue->SetDouble(var.toDouble());
		}
		break;
		case QVariant::String: {
			retValue->SetString(QCefValueConverter::to(var.toString()));
		}
		break;
		case QVariant::List: {
			retValue = QCefValueConverter::to(var.toList());
		}
		break;
		case QVariant::StringList: {
			retValue = QCefValueConverter::to(var.toStringList());
		}
		break;
		case QVariant::Map: {
			retValue = QCefValueConverter::to(var.toMap());
		}
		break;
		default:
			break;
	}
	return retValue;
}

CefRefPtr<CefValue> QCefValueConverter::to( const QVariantMap& v) {
	CefRefPtr<CefValue> retValue = CefValue::Create();
	CefRefPtr<CefDictionaryValue> dicValue = CefDictionaryValue::Create();
	for (QVariantMap::ConstIterator it = v.begin(); it != v.end(); it++) {
		dicValue->SetValue(QCefValueConverter::to(it.key()), QCefValueConverter::to(it.value()));
	}
	retValue->SetDictionary(dicValue);
	return retValue;
}


QVariant QCefValueConverter::from(const CefRefPtr<CefValue>& v) {
	QVariant a;
	a = converCefValueToVariant(v);
	return a;
}

QVariant QCefValueConverter::converCefValueToVariant(CefRefPtr<CefValue> arg)
{
	switch (arg->GetType())
	{
	case VTYPE_INT:
		return QVariant(arg->GetInt());
		break;
	case VTYPE_STRING:
		return QVariant::fromValue(QCefValueConverter::from(arg->GetString()));
		break;
	case VTYPE_BINARY:
	{
		CefRefPtr<CefBinaryValue> binaryValue = arg->GetBinary();
		QByteArray qtby;
		qtby.resize(binaryValue->GetSize());
		binaryValue->GetData(qtby.data(), qtby.size(), 0);
		return QVariant(qtby);
	}
	break;
	case VTYPE_BOOL:
		return QVariant(arg->GetBool());
		break;
	case VTYPE_DOUBLE:
		return QVariant(arg->GetDouble());
		break;
	case VTYPE_LIST:
		return QVariant(convertFromCefListToVariantList(arg));
		break;
	case VTYPE_DICTIONARY:
		return QVariant(convertFromCefValueToVariantMap(arg));
		break;
	case VTYPE_NULL:
	case VTYPE_INVALID:
	default:
		return QVariant();
	}

	return QVariant();
}

QVariantList QCefValueConverter::convertFromCefListToVariantList(const  CefRefPtr<CefValue>& arg)
{
	if (arg->GetType() != VTYPE_LIST || arg->GetList() == NULL)
	{
		return QVariantList();
	}

	QVariantList result;
	const size_t count = arg->GetList()->GetSize();
	for (size_t i = 0; i < count; ++i)
	{
		CefRefPtr<CefValue> v = arg->GetList()->GetValue(i);
		if (v)
		{
			result.append(QCefValueConverter::converCefValueToVariant(v));
		}
	}

	return result;
}

QVariantMap QCefValueConverter::convertFromCefValueToVariantMap( CefRefPtr<CefValue>& arg)
{
	QVariantMap result;

	if (arg->GetType() != VTYPE_DICTIONARY || arg->GetDictionary() == NULL)
	{
		return result;
	}

	CefDictionaryValue::KeyList keys;
	if (!arg->GetDictionary()->GetKeys(keys))
	{
		return result;
	}

	for (int i = 0; i != (int)keys.size(); ++i)
	{
		CefString key = keys[i];
		CefRefPtr<CefValue> v = arg->GetDictionary()->GetValue(key);
		if (v)
		{
			result.insert(QCefValueConverter::from(key), QCefValueConverter::converCefValueToVariant(v));
		}
	}
	return result;
}