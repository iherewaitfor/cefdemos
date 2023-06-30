#pragma once
#include "qcefvalue_qvar_convert.h"
#include "qcefipcvalue.h"

void cefv8bind_protcool::CefMetaMethod::pack(CefRefPtr<CefListValue> protocolMsg)
{
	protocolMsg->SetInt(0, methodType);
	protocolMsg->SetInt(1, methodIndex);
	protocolMsg->SetString(2, QCefValueConverter::to(methodName));
	protocolMsg->SetValue(3, QCefValueConverter::to(parameterTypes)); 
	protocolMsg->SetString(4, QCefValueConverter::to(returnTypeName));
}

bool cefv8bind_protcool::CefMetaMethod::unPack(const CefRefPtr<CefListValue> protocolMsg)
{
	methodType = static_cast<QMetaMethod::MethodType>(protocolMsg->GetInt(0));
	methodIndex = protocolMsg->GetInt(1);
	methodName = QString::fromStdString(protocolMsg->GetString(2).ToString());
	parameterTypes = QCefValueConverter::converCefValueToVariant(protocolMsg->GetValue(3)).toStringList();
	returnTypeName = QString::fromStdString(protocolMsg->GetString(4).ToString());
	return true;
}

void cefv8bind_protcool::CefMetaProperty::pack(CefRefPtr<CefListValue> protocolMsg)
{
	protocolMsg->SetInt(0, propertyIndex);
	protocolMsg->SetString(1, QCefValueConverter::to(propertyName));
	protocolMsg->SetString(2, QCefValueConverter::to(propertyTypeName));
	protocolMsg->SetValue(3, QCefValueConverter::to(propertyValue));
}

bool cefv8bind_protcool::CefMetaProperty::unPack(const CefRefPtr<CefListValue> protocolMsg)
{
	propertyIndex = protocolMsg->GetInt(0);
	propertyName = QCefValueConverter::from(protocolMsg->GetString(1));
	propertyTypeName = QCefValueConverter::from(protocolMsg->GetString(2));
	propertyValue = QCefValueConverter::from(protocolMsg->GetValue(3));
	cef_propertyValue = protocolMsg->GetValue(3);

	return true;
}

void cefv8bind_protcool::CefMetaObject::pack(CefRefPtr<CefListValue> protocolMsg)
{
	CefRefPtr<CefListValue> cef_metaMethods = CefListValue::Create();
	for (int i = 0; i < metaMethods.count(); ++i)
	{
		CefRefPtr<CefListValue> protocolItem = CefListValue::Create();
		CefMetaMethod item = metaMethods.at(i);
		item.pack(protocolItem);

		cef_metaMethods->SetList(i, protocolItem);

	}

	CefRefPtr<CefListValue> cef_metaPropertys = CefListValue::Create();
	for (int i = 0; i < metaProperties.count(); ++i)
	{
		CefRefPtr<CefListValue> cef_propItem = CefListValue::Create();
		CefMetaProperty item = metaProperties.at(i);
		item.pack(cef_propItem);

		cef_metaPropertys->SetList(i, cef_propItem);
	}

	protocolMsg->SetString(0, QCefValueConverter::to(objectName));
	protocolMsg->SetString(1, QCefValueConverter::to(className));
	protocolMsg->SetInt(2, objectId);
	protocolMsg->SetInt(3, parentId);
	protocolMsg->SetList(4, cef_metaMethods);
	protocolMsg->SetList(5, cef_metaPropertys);
	
}

bool cefv8bind_protcool::CefMetaObject::unPack(const CefRefPtr<CefListValue> protocolMsg)
{
	objectName	= QString::fromStdString(protocolMsg->GetString(0).ToString());
	className	= QString::fromStdString(protocolMsg->GetString(1).ToString());
	objectId = protocolMsg->GetInt(2);
	parentId = protocolMsg->GetInt(3);

	CefRefPtr<CefListValue> cef_metaMethods = protocolMsg->GetList(4);
	for (size_t i = 0; i < cef_metaMethods->GetSize(); ++i)
	{
		CefMetaMethod item;
		if (item.unPack(cef_metaMethods->GetList(i)))
		{
			metaMethods.append(item);
		}
	}

	CefRefPtr<CefListValue> cef_metaPropertys = protocolMsg->GetList(5);
	for (size_t i = 0; i < cef_metaPropertys->GetSize(); ++i)
	{
		CefMetaProperty item;
		if (item.unPack(cef_metaPropertys->GetList(i)))
		{
			metaProperties.append(item);
		}
	}

	return true;
}

CefRefPtr<CefValue> cefv8bind_protcool::CefMetaType::pack()
{
	CefRefPtr<CefListValue>	listValue = CefListValue::Create();
	listValue->SetString(0, "{A8321A0C-B23B-CAAE-9E94-2716BE8C707F}");
	listValue->SetString(1, QCefValueConverter::to(typeName));
	listValue->SetValue(2, value);

	CefRefPtr<CefValue> retValue = CefValue::Create();
	retValue->SetList(listValue);

	return retValue;
}

bool cefv8bind_protcool::CefMetaType::unPack(const CefRefPtr<CefValue> protocolMsg)
{
	if (!isValid(protocolMsg))
	{
		return false;
	}

	CefRefPtr<CefListValue>	listValue = protocolMsg->GetList();
	typeName = QCefValueConverter::from(listValue->GetString(1));
	value = listValue->GetValue(2);

	return true;
}

bool cefv8bind_protcool::CefMetaType::isValid(const CefRefPtr<CefValue> protocolMsg)
{
	if (protocolMsg->GetType() == VTYPE_LIST 
		&& protocolMsg->GetList()->GetSize() == 3
		)
	{
		CefRefPtr<CefListValue>	listValue = protocolMsg->GetList();
		if (listValue->GetValue(0)->GetType() == VTYPE_STRING
			&& listValue->GetString(0) == "{A8321A0C-B23B-CAAE-9E94-2716BE8C707F}"
			&& listValue->GetValue(1)->GetType() == VTYPE_STRING
			)
		{
			return true;
		}
	}

	return false;
}