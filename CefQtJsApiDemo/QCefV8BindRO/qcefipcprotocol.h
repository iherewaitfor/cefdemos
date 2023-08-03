﻿#pragma once
#include <QtCore>
#include "include/cef_process_message.h"
#include "include/cef_values.h"

#include "qcefipcvalue.h"


namespace cefv8bind_protcool
{
#define QCEF_IPCMSG_NAME(struct_name) static char* message_name() { return  "qcefv8bind_"#struct_name; }	\
										virtual char* message_name_() { return struct_name::message_name(); }

	struct  PackageBase
	{
		QCEF_IPCMSG_NAME(PackageBase)

		virtual void pack(CefRefPtr<CefListValue> protocolMsg)
		{

		}

		virtual bool unPack(const CefRefPtr<CefListValue> protocolMsg)
		{
			return true;
		}

		CefRefPtr<CefProcessMessage> makeIPCMessage()
		{
			CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create(message_name_());
			pack(msg->GetArgumentList());
			return msg;
		}
	};

	struct CefApiMetaDatasReq : public PackageBase
	{
		QCEF_IPCMSG_NAME(CefApiMetaDatasReq)

		void pack(CefRefPtr<CefListValue> protocolMsg)
		{
			protocolMsg->SetInt(0, render_pid);
		}

		bool unPack(const CefRefPtr<CefListValue> protocolMsg)
		{
			render_pid = protocolMsg->GetInt(0);

			return true;
		}

		int render_pid;
	};
	struct CefApiMetaDatasResponse : public PackageBase
	{
		QCEF_IPCMSG_NAME(CefApiMetaDatasResponse)

		void pack(CefRefPtr<CefListValue> protocolMsg)
		{
			for (int i = 0; i < cef_metaObjects.count(); ++i)
			{
				CefRefPtr<CefListValue> cef_object = CefListValue::Create();
				CefMetaObject meta_object = cef_metaObjects.at(i);
				meta_object.pack(cef_object);

				protocolMsg->SetList(i, cef_object);
			}
		}

		bool unPack(const CefRefPtr<CefListValue> protocolMsg)
		{
			for (size_t i = 0; i < protocolMsg->GetSize(); ++i)
			{
				CefRefPtr<CefListValue> cef_object = protocolMsg->GetList(i);

				CefMetaObject meta_object;
				if (meta_object.unPack(cef_object))
				{
					cef_metaObjects.append(meta_object);
				}
			}

			return true;
		}

		QList<CefMetaObject>	cef_metaObjects;
	};

}
