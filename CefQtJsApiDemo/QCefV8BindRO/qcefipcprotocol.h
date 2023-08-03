#pragma once
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

}
