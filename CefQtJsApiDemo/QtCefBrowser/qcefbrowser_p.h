#pragma once
#include <Windows.h>
#include "qcefbrowser.h"
#include "include/cef_app.h"
#include "simple_handler.h"
#include <QtCore>
class QCefBrowser;
class SimpleHandler;
class QCefBrowserPrivate : public QObject,
    public std::tr1::enable_shared_from_this<QCefBrowserPrivate>
{
    Q_OBJECT
public:
    QCefBrowser* q_ptr;
    QString m_url;
    HWND m_parent;
    CefRefPtr<CefBrowser> m_browser;
    volatile unsigned long m_closing;
    int m_uniqueWindowId;
public:
    QCefBrowserPrivate(QCefBrowser* q, QString url);
    ~QCefBrowserPrivate();

    CefRefPtr<CefBrowserHost> browserHost();
    void createBrowser();
    void closeBrowser();
    void OnAfterCreated(CefRefPtr<CefBrowser> browser);
    void OnBeforeClose();
    void OnClosing(CefRefPtr<CefBrowser> browser);

    void OnAfterCreatedPoppup(CefRefPtr<CefBrowser> browser);
    void OnBeforeClosePoppup(CefRefPtr<CefBrowser> browser);
signals:
    void afterCreated(CefRefPtr<CefBrowser> browser);
    void beforeClose();
    void closing(CefRefPtr<CefBrowser> browser);
    void afterCreatedPoppup(CefRefPtr<CefBrowser> browser);
    void beforeClosePoppup(CefRefPtr<CefBrowser> browser);
private slots:
    void OnAfterCreatedSlot(CefRefPtr<CefBrowser> browser);
    void OnBeforeCloseSlot();
    void OnClosingSlot(CefRefPtr<CefBrowser> browser);
    void afterCreatedPoppupSlot(CefRefPtr<CefBrowser> browser);
    void beforeClosePoppupSlot(CefRefPtr<CefBrowser> browser);
};
