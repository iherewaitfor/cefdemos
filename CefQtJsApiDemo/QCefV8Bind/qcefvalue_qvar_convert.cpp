#include "qcefipcvalue.h"
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
	retValue->SetList(list);
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
CefRefPtr<CefV8Value> QCefValueConverter::to(CefRefPtr<CefValue> const& v)
{
	CefRefPtr<CefV8Value> result;
	switch (v->GetType())
	{
	case VTYPE_INVALID:
		result = CefV8Value::CreateUndefined();
		break;
	case VTYPE_NULL:
		result = CefV8Value::CreateNull();
		break;
	case VTYPE_BOOL:
		result = CefV8Value::CreateBool(v->GetBool());
		break;
	case VTYPE_INT:
		result = CefV8Value::CreateInt(v->GetInt());
		break;
	case VTYPE_DOUBLE:
		result = CefV8Value::CreateDouble(v->GetDouble());
		break;
	case VTYPE_STRING:
	{
		result = CefV8Value::CreateString(v->GetString());
	}
	break;
	case VTYPE_DICTIONARY:
	{
		CefRefPtr<CefV8Value> v8ValueMap = CefV8Value::CreateObject(nullptr, nullptr);
		CefDictionaryValue::KeyList keys;
		if (v->GetDictionary() && v->GetDictionary()->GetKeys(keys))
		{
			for (CefDictionaryValue::KeyList::size_type i = 0; i < keys.size(); ++i)
			{
				CefRefPtr<CefValue> cefValue = v->GetDictionary()->GetValue(keys[i]);
				v8ValueMap->SetValue(keys[i], QCefValueConverter::to(cefValue), V8_PROPERTY_ATTRIBUTE_READONLY);
			}
		}

		result = v8ValueMap;
	}
	break;
	case VTYPE_LIST:
	{
		CefRefPtr<CefV8Value> v8Values = CefV8Value::CreateArray(v->GetList()->GetSize());
		for (size_t i = 0; i < v->GetList()->GetSize(); ++i)
		{
			CefRefPtr<CefV8Value> v8Value = QCefValueConverter::to(v->GetList()->GetValue(i));
			if (v8Value)
			{
				v8Values->SetValue(i, v8Value);
			}
		}
		result = v8Values;
	}
	break;
	default:
		break;
	}

	return result;
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

CefRefPtr<CefValue> QCefValueConverter::from(CefRefPtr<CefV8Value> const& v)
{
	CefRefPtr<CefValue> result = CefValue::Create();	
	if (v->IsString())
	{
		result->SetString(v->GetStringValue());
	}
	else if (v->IsUInt() || v->IsInt())
	{
		result->SetInt(v->IsUInt() ? v->GetUIntValue() : v->GetIntValue());
	}
	else if (v->IsNull())
	{
		result->SetNull();
	}
	else if (v->IsDouble())
	{
		result->SetDouble(v->GetDoubleValue());
	}
	else if (v->IsBool())
	{
		result->SetBool(v->GetBoolValue());
	}
	else if (v->IsArray())
	{
		CefRefPtr<CefListValue> listValue = CefListValue::Create();
		result->SetList(listValue);
		for (int i = 0; i < v->GetArrayLength(); ++i)
		{
			listValue->SetValue(i, QCefValueConverter::from(v->GetValue(i)));
		}
	}
	else if (v->IsObject())
	{
		CefRefPtr<CefDictionaryValue> mapCefValue = CefDictionaryValue::Create();
		result->SetDictionary(mapCefValue);

		std::vector<CefString> keys;
		if (v->GetKeys(keys))
		{
			std::vector<CefString>::const_iterator iter = keys.begin();
			while (iter != keys.end())
			{
				mapCefValue->SetValue(*iter, QCefValueConverter::from(v->GetValue(*iter)));
				++iter;
			}
		}
	}

	return result;
}
