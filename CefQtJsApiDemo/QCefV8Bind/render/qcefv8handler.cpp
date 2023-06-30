#include "qcefv8handler.h"
#include "include/wrapper/cef_helpers.h"
#include "../qcefipcvalue.h"
#include "../qcefv8objecthelper.h"
#include "../qcefv8bindutility.h"
#include "asyncmethodcallback.h"

using namespace cefv8bind_protcool;

QCefV8Handler::QCefV8Handler(CefRefPtr<CefFrame> frame)
: m_frame(frame)
, m_asynCallbackMgr(new AsyncCefMethodCallbackMgr(0, frame->GetIdentifier()))
{
	
}

QCefV8Handler::~QCefV8Handler()
{

}
void QCefV8Handler::clear()
{
    CEF_REQUIRE_RENDERER_THREAD();
    m_frame = nullptr;
}

CefRefPtr<CefFrame> QCefV8Handler::getFrame()
{
	return m_frame;
}

bool QCefV8Handler::Execute(const CefString& name, CefRefPtr<CefV8Value> v8Object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception)
{

	QSharedPointer<QObject>	itemObj;
	CefRefPtr<CefBaseRefCounted> userData = v8Object->GetUserData();
	if (userData)
	{
		itemObj = static_cast<QCefV8ObjectHolder<QSharedPointer<QObject>>*>(userData.get())->wrapObject();
	}

	CefRefPtr<CefV8Value> funcV8Value = v8Object->GetValue(name);
	if (funcV8Value == NULL || !funcV8Value->GetUserData())
	{
		return false;
	}

	QSharedPointer<QObject>	metaObj;
	userData = funcV8Value->GetUserData();
	if (userData)
	{
		metaObj = static_cast<QCefV8ObjectHolder<QSharedPointer<QObject>>*>(userData.get())->wrapObject();
	}

	if (metaObj == NULL)
	{
		return false;
	}

	const cefv8bind_protcool::CefMetaMethod& cefMetaMethord = metaObj->property(KCefMetaMethod).value<cefv8bind_protcool::CefMetaMethod>();

	int methodIndex = cefMetaMethord.methodIndex;
	int methodType = cefMetaMethord.methodType;
	if (methodType == QMetaMethod::Method)
	{
		if (itemObj )
		{
			CefRefPtr<CefV8Context> context = CefV8Context::GetCurrentContext();

			quint64  callbackId = 0;
			if (!cefMetaMethord.returnTypeName.isEmpty() && cefMetaMethord.returnTypeName != "void")
			{
				CefRefPtr<CefV8Value> window = context->GetGlobal();
				CefRefPtr<CefV8Value> createPromise = window->GetValue(KPromiseCreatorFunction);
				CefRefPtr<CefV8Value> promiseWrapper = createPromise->ExecuteFunctionWithContext(context, nullptr, CefV8ValueList());

				retval = promiseWrapper->GetValue("p");

				CefRefPtr<CefV8Value> resolve = promiseWrapper->GetValue("resolve");
				CefRefPtr<CefV8Value> reject = promiseWrapper->GetValue("reject");

				QSharedPointer<AsyncCefMethodCallback> asyncCallback = QSharedPointer<AsyncCefMethodCallback>(
					new AsyncCefMethodCallback(context, resolve, reject, context->GetFrame()->GetIdentifier(), name)
					);
				callbackId = m_asynCallbackMgr->saveAsyncMethodCallback(asyncCallback);
			}
			else
			{
				retval = CefV8Value::CreateInt(0);
			}
			cefv8bind_protcool::InvokeReq req;
			req.objctId = metaObj->property(KObjectId).toInt();
			req.methodName = name;
			req.methodIndex = methodIndex;
			req.callBackId = callbackId;
			req.methodArgs = toProcessMessage(arguments);

			sendIPCMessage(context, req);

		}

		return true;
	}

	return false;
}
void QCefV8Handler::on_cefMetaDatasResponse(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefProcessMessage> message)
{
	if (m_frame == NULL)
	{
		return;
	}
	CefRefPtr<CefV8Context> context = m_frame->GetV8Context();

	CefApiMetaDatasResponse ipc_protocol;
	if (!ipc_protocol.unPack(message->GetArgumentList()))
	{
		return;
	}
	V8ContextCaller auto_caller(context);
	QCefV8ObjectHelper objectHelper;
	CefRefPtr<CefV8Value> rootV8Obj = objectHelper.bindV8Objects(ipc_protocol.cef_metaObjects, context, this);
	rootV8Obj;
}

CefRefPtr<CefListValue> QCefV8Handler::toProcessMessage(const CefV8ValueList& source)
{
	CefRefPtr<CefListValue> result = CefListValue::Create();
	CefV8ValueList::const_iterator iter = source.begin();
	for(int i = 0; i < (int)source.size(); i++)
	{
		CefRefPtr<CefV8Value> sourceItem = source[i];
		if (!sourceItem->IsFunction())
		{
			result->SetValue(i, QCefValueConverter::from(sourceItem));
		}
	}
	return result;
}

void QCefV8Handler::sendIPCMessage(CefRefPtr<CefV8Context> context,
	cefv8bind_protcool::PackageBase& ipcMsg){
	CEF_REQUIRE_RENDERER_THREAD();
	if (context->GetFrame())
	{
		context->GetFrame()->SendProcessMessage(PID_BROWSER, ipcMsg.makeIPCMessage());
	}
}
