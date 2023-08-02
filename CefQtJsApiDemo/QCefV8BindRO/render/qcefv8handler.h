#pragma once
#include "include/cef_v8.h"
#include "qcefv8bindrenderdelegate.h"
#include "../qcefipcprotocol.h"
#include "asyncmethodcallback.h"
class QCefV8SignalManager : public virtual CefBaseRefCounted
{
public:
	QCefV8SignalManager()
	{
	}

	~QCefV8SignalManager()
	{
	}

	bool connectSignal(const CefV8ValueList& arguments, CefRefPtr<CefV8Value>& retval, CefString& exception, CefRefPtr<CefV8Handler> v8Handler);
	bool disConnectSignal(CefRefPtr<CefV8Value> v8Object, CefRefPtr<CefV8Context> context);
	void emitSignal(const cefv8bind_protcool::EmitSignalMsg& msg, CefRefPtr<CefV8Context> context);
	void emitRenderSignal(const QString& signalName, CefRefPtr<CefListValue> methordArgs, CefRefPtr<CefV8Context> context);

	void clear();

private:
	IMPLEMENT_REFCOUNTING(QCefV8SignalManager);

	QString _getKey(quint32 objectId, quint32 methodIndex);
	QString _getRenderSignalKey(const QString& signalName);
	CefV8ValueList _convertToV8Args(const CefRefPtr<CefListValue>& methordArgs);
	CefRefPtr<CefV8Value>	_createDisConnectV8Object(const QString& key, const CefRefPtr<CefV8Value> slot, CefRefPtr<CefV8Handler> v8Handler);
private:
	QMap<QString, QList<CefRefPtr<CefV8Value>>>	m_slots;
};



class QClientRenderDelegate;
class QCefV8Handler : public CefV8Handler
{
public:
	friend QClientRenderDelegate;

	QCefV8Handler(CefRefPtr<CefFrame> frame);
	virtual ~QCefV8Handler();
	void clear();

	CefRefPtr<CefFrame>	getFrame();
	

public:
	void onInvokeResponse(CefRefPtr<CefProcessMessage> message, CefRefPtr<CefV8Context> context);
	void onEmitSignalMsg(CefRefPtr<CefProcessMessage> message);

private:
	//CefV8Handler
	virtual bool Execute(const CefString& name,
		CefRefPtr<CefV8Value> v8Object,
		const CefV8ValueList& arguments,
		CefRefPtr<CefV8Value>& retval,
		CefString& exception);
private:
	CefRefPtr<CefListValue> toProcessMessage(const CefV8ValueList& source);
	void sendIPCMessage(CefRefPtr<CefV8Context> context,
		cefv8bind_protcool::PackageBase& ipc_protocol);

private:
    CefRefPtr<CefFrame> m_frame;
	QPointer<AsyncCefMethodCallbackMgr> m_asynCallbackMgr;
	CefRefPtr<QCefV8SignalManager>	m_v8SignalMgr;

	IMPLEMENT_REFCOUNTING(QCefV8Handler);
};


