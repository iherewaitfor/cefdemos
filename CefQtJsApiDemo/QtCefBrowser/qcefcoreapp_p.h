#ifndef QCEFCOREAPP_P_H
#define QCEFCOREAPP_P_H
#pragma once
#include "include/cef_app.h"


#include <set>
#include <map>
#include <QtCore>
#include "qcefbrowser.h"
#include "qcefcoreapp.h"
#include "qcefbrowserhandlerdelegate.h"
#include "render/qcefrenderhandlerdelegate.h"
#include "window.h"
class QCefCoreApp;
class QCefBrowser;
class QCefCoreAppPrivate : public QObject,  public CefBaseRefCounted
{
    IMPLEMENT_REFCOUNTING(QCefCoreAppPrivate);
    Q_OBJECT;
signals:
    void allClosed();
public:
    QCefCoreApp* q_ptr;
    std::map<int,QSharedPointer<QCefBrowser>> m_browsers;
    std::set<int> m_popupBrowsers;
    QPointer<QObject> m_qApiRootObject;
    std::set<client::BrowserDelegate*> m_browserDelegates;
    std::set<client::RenderDelegate*> m_renderDelegates;
    bool m_bClosing;
    QMap<int, QSharedPointer<CefApi::Window>> m_apiWindows; //cefbrowserId, apiwindow
public:

    QCefCoreAppPrivate(QCefCoreApp* q)
        : q_ptr(q), m_bClosing(false)
    {
        qRegisterMetaType<CefRefPtr<CefBrowser>>("CefRefPtr<CefBrowser>");
        qRegisterMetaType<CefRefPtr<CefFrame>>("CefRefPtr<CefFrame>");
        qRegisterMetaType<CefProcessId>("CefProcessId");
        qRegisterMetaType<CefRefPtr<CefProcessMessage>>("CefRefPtr<CefProcessMessage>");

    }

    ~QCefCoreAppPrivate()
    {
    }

    void addBrowser(int id, QSharedPointer<QCefBrowser> v)
    {
        m_browsers[id] = v;
    }

    void removeBrowser(int id)
    {
        m_browsers.erase(id);

        if (m_browsers.empty() && m_popupBrowsers.empty())
        {
            // to do quit
            quit();
        }
    }
    void addPopupBrowser(int browserId) {
        m_popupBrowsers.insert(browserId);
    }
    void removePopupBrowser(int browserId) {
        m_popupBrowsers.erase(browserId);
        if (m_popupBrowsers.empty() && m_browsers.empty()) {
           quit();
        }
    }
    void addApiWindow(int cefBrowserId, QSharedPointer<CefApi::Window> v)
    {
        m_apiWindows[cefBrowserId] = v;
    }

    void removeApiWindow(int cefBrowserId)
    {
        m_apiWindows.remove(cefBrowserId);
    }
    QSharedPointer<CefApi::Window> getApiWindow(int cefBrowserId) {
        if (m_apiWindows.contains(cefBrowserId)) {
            return m_apiWindows.value(cefBrowserId);
        }
        return nullptr;
    }
    
    void setApiRoot(QPointer<QObject> qApiRootObject) {
        m_qApiRootObject = qApiRootObject;
    }
    QPointer<QObject> getApitRootObject() {
        return m_qApiRootObject;
    }
    std::set<client::BrowserDelegate*> browserDelegates() const { 
        return m_browserDelegates; 
    }

    std::set<client::RenderDelegate*> renderDelegates() const {
        return m_renderDelegates;
    }
public slots:
    void quit() {
        m_bClosing = true;
        CefQuitMessageLoop();
        emit allClosed();
    }
};
#define  qCefCoreAppPrivate() \
QCefCoreApp::getInstance()->d_func()

#endif 

