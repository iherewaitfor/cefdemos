#include "qcefv8handler.h"
#include "include/wrapper/cef_helpers.h"
#include "../qcefipcvalue.h"
#include "../qcefv8objecthelper.h"
#include "../qcefv8bindutility.h"
#include "asyncmethodcallback.h"
#include "../metainvoker.h"

using namespace cefv8bind_protcool;

bool QCefV8SignalManager::connectSignal(const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception, CefRefPtr<CefV8Handler> v8Handler)
{
	if (arguments.size() < 2)
	{
		return false;
	}
	CefRefPtr<CefV8Value> signal = arguments[0];
	CefRefPtr<CefV8Value> slot = arguments[1];
	if (!slot->IsFunction())
	{
		return false;
	}
	if (!signal->IsObject())
	{
		return false;
	}
	QSharedPointer<QObject>	metaObj;
	CefRefPtr<CefBaseRefCounted> userData = signal->GetUserData();
	if (userData)
	{
		metaObj = static_cast<QCefV8ObjectHolder<QSharedPointer<QObject>>*>(userData.get())->wrapObject();
	}
	if (metaObj == NULL)
	{
		return false;
	}
	QString key;
	if (metaObj->property(KRenderV8Object).toBool() && !metaObj->objectName().isEmpty())
	{
		key = _getRenderSignalKey(metaObj->objectName());
	}
	else
	{
		const cefv8bind_protcool::CefMetaMethod  cefMetaMethord = metaObj->property(KCefMetaMethod).value<cefv8bind_protcool::CefMetaMethod>();

		quint32 methodIndex = cefMetaMethord.methodIndex;
		quint32 methodType = cefMetaMethord.methodType;
		quint32 objectId = metaObj->property(KObjectId).toUInt();
		if (methodType != QMetaMethod::Signal)
		{
			return false;
		}
		key = _getKey(objectId, methodIndex);
	}
	if (!m_slots.contains(key))
	{
		QList<CefRefPtr<CefV8Value>> list;
		list << slot;
		m_slots.insert(key, list);
		retval = _createDisConnectV8Object(key, slot, v8Handler);
	}
	else
	{
		QList<CefRefPtr<CefV8Value>> cef_slots = m_slots[key];
		bool isExist = false;
		foreach(CefRefPtr<CefV8Value> cef_slot, cef_slots)
		{
			if (cef_slot->IsSame(slot))
			{
				isExist = true;
				break;
			}
		}

		if (!isExist)
		{
			cef_slots << slot;
			m_slots[key] = cef_slots;
			retval = _createDisConnectV8Object(key, slot, v8Handler);
		}
	}

	return true;
}

bool QCefV8SignalManager::disConnectSignal(CefRefPtr<CefV8Value> v8Object, CefRefPtr<CefV8Context> context)
{
	if (!v8Object->IsObject())
	{
		return false;
	}

	CefRefPtr<CefV8Value> func = v8Object->GetValue(DisConnectSignal);
	if (func == NULL || !func->IsFunction())
	{
		return false;
	}

	QString key;
	CefRefPtr<CefV8Value> slot;
	if (func->GetValue(0) && func->GetValue(0)->IsString()
		&& func->GetValue(1) && func->GetValue(1)->IsObject()
		)
	{
		key = QCefValueConverter::from(func->GetValue(0)->GetStringValue());
		slot = func->GetValue(1);
	}
	else
	{
		return false;
	}

	if (slot && !slot->IsFunction())
	{
		return false;
	}

	if (slot == NULL)
	{
		m_slots.remove(key);
	}
	else
	{
		QList<CefRefPtr<CefV8Value>> cef_slots = m_slots[key];
		QList<CefRefPtr<CefV8Value>>::iterator iter = cef_slots.begin();
		while (iter != cef_slots.end())
		{
			if ((*iter)->IsSame(slot))
			{
				cef_slots.erase(iter);

				if (cef_slots.isEmpty())
				{
					m_slots.remove(key);
				}
				else
				{
					m_slots[key] = cef_slots;
				}

				break;
			}

			++iter;
		}
	}

	return true;
}

void QCefV8SignalManager::emitSignal(const EmitSignalMsg& msg, CefRefPtr<CefV8Context> context)
{
	QString key = _getKey(msg.objectId, msg.methodIndex);
	if (m_slots.contains(key))
	{
		QList<CefRefPtr<CefV8Value>> cef_slots = m_slots[key];

		Q_FOREACH(CefRefPtr<CefV8Value> callBack, cef_slots)
		{
			V8ContextCaller caller(context);
			CefV8ValueList arguments_out = _convertToV8Args(msg.methodArgs);

			// Execute the callback.
			CefRefPtr<CefV8Value> retval = callBack->ExecuteFunctionWithContext(context, nullptr, arguments_out);
			if (retval.get())
			{
				if (retval->IsBool())
					retval->GetBoolValue();
			}
		}
	}
}

void QCefV8SignalManager::emitRenderSignal(const QString& signalName, CefRefPtr<CefListValue> methordArgs, CefRefPtr<CefV8Context> context)
{
	QString key = _getRenderSignalKey(signalName);
	if (m_slots.contains(key))
	{
		QList<CefRefPtr<CefV8Value>> cef_slots = m_slots[key];

		foreach(CefRefPtr<CefV8Value> callBack, cef_slots)
		{
			V8ContextCaller caller(context);
			CefV8ValueList arguments_out = _convertToV8Args(methordArgs);

			// Execute the callback.
			CefRefPtr<CefV8Value> retval = callBack->ExecuteFunctionWithContext(context, nullptr, arguments_out);
			if (retval.get())
			{
				if (retval->IsBool())
					retval->GetBoolValue();
			}
		}
	}
}

void QCefV8SignalManager::clear()
{
	m_slots.clear();
}

QString QCefV8SignalManager::_getKey(quint32 objUid, quint32 methordIndex)
{
	return QString("signal_%1_%2").arg(objUid).arg(methordIndex);
}

QString QCefV8SignalManager::_getRenderSignalKey(const QString& signalName)
{
	return QString("signal_%1").arg(signalName);
}

CefV8ValueList QCefV8SignalManager::_convertToV8Args(const CefRefPtr<CefListValue>& methordArgs)
{
	CefV8ValueList arguments_out;

	CefRefPtr<CefValue> toValue = CefValue::Create();
	toValue->SetList(methordArgs);

	CefRefPtr<CefV8Value> v8Values = QCefValueConverter::to(toValue);
	if (v8Values->IsArray())
	{
		for (int i = 0; i < v8Values->GetArrayLength(); ++i)
		{
			arguments_out.push_back(v8Values->GetValue(i));
		}
	}

	return arguments_out;
}

CefRefPtr<CefV8Value> QCefV8SignalManager::_createDisConnectV8Object(const QString& key, const CefRefPtr<CefV8Value> slot, CefRefPtr<CefV8Handler> v8Handler)
{
	CefRefPtr<CefV8Value> methodV8Object = CefV8Value::CreateFunction(DisConnectSignal, v8Handler);
	methodV8Object->SetValue(0, CefV8Value::CreateString(QCefValueConverter::to(key)));
	methodV8Object->SetValue(1, slot);

	CefRefPtr<CefV8Value> v8Object = CefV8Value::CreateObject(nullptr, nullptr);
	v8Object->SetValue(DisConnectSignal, methodV8Object, V8_PROPERTY_ATTRIBUTE_NONE);
	return v8Object;
}

//////////////////////////////////////////////////////////////////////////


QCefV8Handler::QCefV8Handler(CefRefPtr<CefFrame> frame)
: m_frame(frame)
, m_v8SignalMgr(new QCefV8SignalManager())
, m_asynCallbackMgr(new AsyncCefMethodCallbackMgr(0, frame->GetIdentifier()))
{
	
}

QCefV8Handler::~QCefV8Handler()
{

}
void QCefV8Handler::clear()
{
    CEF_REQUIRE_RENDERER_THREAD();
	m_v8SignalMgr->clear();
	m_v8SignalMgr = nullptr;
	if (m_asynCallbackMgr)
	{
		delete m_asynCallbackMgr;
	}
    m_frame = nullptr;
}

CefRefPtr<CefFrame> QCefV8Handler::getFrame()
{
	return m_frame;
}

bool QCefV8Handler::Execute(const CefString& name, CefRefPtr<CefV8Value> v8Object, const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception)
{
	if (m_frame == NULL)
	{
		return false;
	}

	if (name == ConnectSignal && m_v8SignalMgr)
	{
		m_v8SignalMgr->connectSignal(arguments, retval, exception, this);
		return true;
	}
	else if (name == DisConnectSignal && m_v8SignalMgr)
	{
		bool ret = m_v8SignalMgr->disConnectSignal(v8Object, CefV8Context::GetCurrentContext());
		retval = CefV8Value::CreateBool(ret);
		return true;
	}
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
	QString readFlag = InitSdkReadyFlag;
	{
		CefRefPtr<CefV8Value> v8Value = CefV8Value::CreateObject(nullptr, nullptr);

		QSharedPointer<QObject> obj = QSharedPointer<QObject>(new QObject());
		obj->setObjectName(readFlag);
		obj->setProperty(KRenderV8Object, true);
		v8Value->SetUserData(new QCefV8ObjectHolder<QSharedPointer<QObject>>(obj));

		const CefV8Value::PropertyAttribute attributes = static_cast<CefV8Value::PropertyAttribute>(V8_PROPERTY_ATTRIBUTE_READONLY | V8_PROPERTY_ATTRIBUTE_DONTENUM | V8_PROPERTY_ATTRIBUTE_DONTDELETE);
		context->GetGlobal()->SetValue(QCefValueConverter::to(readFlag), v8Value, attributes);
	}
	m_v8SignalMgr->emitRenderSignal(OnInitSdkOKNotify, CefListValue::Create(), context);
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

void QCefV8Handler::onInvokeResponse(CefRefPtr<CefProcessMessage> message, CefRefPtr<CefV8Context> context)
{
	if (m_frame == NULL)
	{
		return;
	}
	InvokeResp rsp;
	if (!rsp.unPack(message->GetArgumentList()))
	{
		return;
	}
	QSharedPointer<AsyncCefMethodCallback> callback = m_asynCallbackMgr->takeAsyncMethodCallback(rsp.callBackId);
	if (!callback)
	{
		return;
	}
	if (!rsp.invokeResult)
	{
		callback->fail("fail exception");
		return;
	}
	if (context->GetFrame()->GetIdentifier() == callback->frameId()
		&& context->Enter()
		)
	{
		CefRefPtr<CefV8Value> retV8Value = QCefValueConverter::to(rsp.returnValue);
		context->Exit();
		callback->success(retV8Value);
	}
}

void QCefV8Handler::onEmitSignalMsg(CefRefPtr<CefProcessMessage> message)
{
	if (m_frame == NULL)
	{
		return;
	}
	EmitSignalMsg msg;
	if (msg.unPack(message->GetArgumentList()))
	{
		m_v8SignalMgr->emitSignal(msg, m_frame->GetV8Context());
	}
}
