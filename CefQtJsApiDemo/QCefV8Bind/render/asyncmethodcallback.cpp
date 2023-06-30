#include "asyncmethodcallback.h"
#include <QtCore/QSharedPointer>
#include "../qcefv8bindapp_p.h"

const char KPromiseCreatorFunction[] = "qCreatePromise";
const char KPromiseCreatorScript[] = ""
"function qCreatePromise() {"
"   var result = {};"
"   var promise = new Promise(function(resolve, reject) {"
"       result.resolve = resolve; result.reject = reject;"
"   });"
"   result.p = promise;"
"   return result;"
"}";

AsyncCefMethodCallback::AsyncCefMethodCallback(CefRefPtr<CefV8Context> context, const CefRefPtr<CefV8Value>& res, const CefRefPtr<CefV8Value>& rej, int64_t frameId, const CefString& sName)
:  m_timeOutTime(3000)
{
    _context = context;
    _resolve = res;
    _reject = rej;
    _frameId = frameId;
    _sName = sName;
    _callTime = GetTickCount64();
}

AsyncCefMethodCallback::~AsyncCefMethodCallback()
{

}

void AsyncCefMethodCallback::success(const CefRefPtr<CefV8Value>& result)
{
    if (_resolve.get() && _context.get() && _context->Enter())
    {
        CefV8ValueList args;
        args.push_back(result);
        _resolve->ExecuteFunction(nullptr, args);
        _context->Exit();
    }
}

void AsyncCefMethodCallback::fail(const CefString& exception)
{
    if (_reject.get() && _context.get() && _context->Enter())
    {
        CefV8ValueList args;
        args.push_back(CefV8Value::CreateString(exception));
        _reject->ExecuteFunction(nullptr, args);
        _context->Exit();
    }
}

qint64 AsyncCefMethodCallbackMgr::m_asyncCefMethodCallbackId = 0;

AsyncCefMethodCallbackMgr::AsyncCefMethodCallbackMgr(QObject* parent, quint64 frameId)
    : QObject(parent)
    , m_frameId(frameId)
{
}

AsyncCefMethodCallbackMgr::~AsyncCefMethodCallbackMgr()
{
    m_asyncCefMethodCallbacks.clear();
}


quint64 AsyncCefMethodCallbackMgr::_guidCreater()
{
    return InterlockedIncrement64(&m_asyncCefMethodCallbackId);
}

quint64 AsyncCefMethodCallbackMgr::saveAsyncMethodCallback(QSharedPointer<AsyncCefMethodCallback> cb)
{
    qint64 callbackId = _guidCreater();
    m_asyncCefMethodCallbacks.insert(callbackId, cb);

    return callbackId;
}

QSharedPointer<AsyncCefMethodCallback> AsyncCefMethodCallbackMgr::takeAsyncMethodCallback(qint64 id)
{
    return m_asyncCefMethodCallbacks.take(id);
}
