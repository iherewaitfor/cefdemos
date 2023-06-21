﻿#pragma once
#include <Windows.h>
#include "qcefbrowser.h"
#include "include/cef_app.h"
#include "simple_handler.h"
class QCefBrowser;

class QCefBrowserPrivate
    :  public QObject, public std::tr1::enable_shared_from_this<QCefBrowserPrivate>
{
    Q_OBJECT;
public:
    QCefBrowser* q_ptr;

    QString m_url;

    HWND m_parent;

    CefRefPtr<CefBrowser> m_browser;
    bool m_isClosing;
    int m_uniqueWindowId;
public:
    QCefBrowserPrivate(QCefBrowser* q, QString url);
    ~QCefBrowserPrivate();
    Q_INVOKABLE void createBroserInVoke();

    CefRefPtr<CefBrowserHost> browserHost();
    void createBrowser();
    void closeBroser();
    void OnAfterCreated(CefRefPtr<CefBrowser> browser);
    void OnClosing(CefRefPtr<CefBrowser> browser);
};