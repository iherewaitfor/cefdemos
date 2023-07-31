#pragma once
#include <QtCore>
#include "include/cef_v8.h"

extern const char KPromiseCreatorFunction[];
extern const char KPromiseCreatorScript[];

class AsyncCefMethodCallback
{
public:
    AsyncCefMethodCallback(CefRefPtr<CefV8Context> context, const CefRefPtr<CefV8Value>& res, const CefRefPtr<CefV8Value>& rej, int64_t frameId, const CefString& sName);
    ~AsyncCefMethodCallback();

    void success(const CefRefPtr<CefV8Value>& result);
    void fail(const CefString& exception);

    int64_t frameId() const { return _frameId; };
    const CefString& name() const { return _sName; }
    uint64_t callTime() const { return _callTime; }
    void setTimeOutTime(uint64_t t) { m_timeOutTime = t; }
    uint64_t getTimeOutTime() {return m_timeOutTime;}

private:
    CefRefPtr<CefV8Context> _context;
    CefRefPtr<CefV8Value> _resolve;
    CefRefPtr<CefV8Value> _reject;
    int64_t _frameId;
    CefString _sName;
    uint64_t _callTime;
    uint64_t m_timeOutTime; //3s
};

class AsyncCefMethodCallbackMgr : public QObject
{
	Q_OBJECT;

public:
    AsyncCefMethodCallbackMgr(QObject *parent, quint64 frameId);
    ~AsyncCefMethodCallbackMgr();

    quint64 saveAsyncMethodCallback(QSharedPointer<AsyncCefMethodCallback> cb);
    QSharedPointer<AsyncCefMethodCallback> takeAsyncMethodCallback(qint64 id);

protected:
    quint64 _guidCreater();

private:
	static qint64 m_asyncCefMethodCallbackId;

    QMap<qint64, QSharedPointer<AsyncCefMethodCallback>> m_asyncCefMethodCallbacks;
    quint64 m_frameId;
};

