#pragma once
#include <Windows.h>
#include "qcefbrowser.h"
#include "include/cef_app.h"
#include "simple_handler.h"
#include <QtCore>
#include "browser/qcefbrowserwindow.h"
#include "browser/browser_struct.h"
class QCefBrowser;
class SimpleHandler;
class QCefBrowserPrivate : public QObject,
    public std::tr1::enable_shared_from_this<QCefBrowserPrivate>
{
    Q_OBJECT
public:
    QCefBrowser* q_ptr;

    BrowserWindowOptions m_windowOptions;

    QString m_startUpUrl;
    QString m_url;
    HWND m_parent;

    CefRefPtr<SimpleHandler> m_clientHandler;
    volatile unsigned long m_closing;
    int m_Id;//只是QCefBrowser的id，创建就有。
    int m_browserId; //实际的browser创建后才有。

    QCefBrowserWindow* m_hostWindow;
    std::vector<QSharedPointer<QCefBrowserChildWindow>> m_hookChildWindows;

public:
    QCefBrowserPrivate(QCefBrowser* q);
    ~QCefBrowserPrivate();

    void onHostWindowWillClose();
    void onHostWindowDestroy();
    bool modalMode() const;

    void createHostWindow();
    void createBrowser();
    void closeBrowser();
    void initWindowOptions(const BrowserWindowOptions& options);
    void initWindow();
    void setSize();
    void hookChildWindows();
    bool findHookedChildWindow(HWND window);

    void OnAfterCreated(CefRefPtr<CefBrowser> browser);
    void OnClosing(CefRefPtr<CefBrowser> browser);
    void OnBeforeClose(CefRefPtr<CefBrowser> browser);

    void OnAfterCreatedPoppup(CefRefPtr<CefBrowser> browser);
    void OnBeforeClosePoppup(CefRefPtr<CefBrowser> browser);
signals:
    void afterCreated(int browserId);
    void closing(int browserId);
    void beforeClose(int browserId);

    void afterCreatedPoppup(int browserId);
    void beforeClosePoppup(int browserId);

private slots:
    void OnAfterCreatedSlot(int browserId);
    void OnBeforeCloseSlot(int browserId);
    void OnClosingSlot(int browserId);

    void afterCreatedPoppupSlot(int browserId);
    void beforeClosePoppupSlot(int browserId);
};
