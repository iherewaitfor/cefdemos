#ifndef QCEFCOREAPP_P_H
#define QCEFCOREAPP_P_H
#pragma once
#include "include/cef_app.h"


#include <set>
#include <map>
#include <QtCore>
#include "qcefbrowser.h"
#include "qcefcoreapp.h"
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
    std::set<QPointer<QCefBrowser>> m_browsers;
    QList<CefRefPtr<CefBrowser>> m_popupBrowsers;
    QPointer<QObject> m_qApiRootObject;
public:

    QCefCoreAppPrivate(QCefCoreApp* q)
        : q_ptr(q)
    {
        qRegisterMetaType<CefRefPtr<CefBrowser>>("CefRefPtr<CefBrowser>");
    }

    ~QCefCoreAppPrivate()
    {
    }

    void addBrowser(QPointer<QCefBrowser> v)
    {
        m_browsers.insert(v);
    }

    void removeBrowser(QPointer<QCefBrowser> v)
    {
        m_browsers.erase(v);

        if (m_browsers.empty() && m_popupBrowsers.isEmpty())
        {
            // to do quit
            quit();
        }
    }
    void addPopupBrowser(CefRefPtr<CefBrowser> browser) {
        m_popupBrowsers.push_back(browser);
    }
    void removePopupBrowser(CefRefPtr<CefBrowser> browser) {
        for (int i = 0; i < m_popupBrowsers.size(); i++) {
            if (m_popupBrowsers.at(i)->GetIdentifier() == browser->GetIdentifier()) {
                m_popupBrowsers.removeAt(i);
                break;
            }
        }
        if (m_popupBrowsers.isEmpty() && m_browsers.empty()) {
           quit();
        }
    }
    void setApiRoot(QPointer<QObject> qApiRootObject) {
        m_qApiRootObject = qApiRootObject;
    }
    QPointer<QObject> getApitRootObject() {
        return m_qApiRootObject;
    }
public slots:
    void quit() {
        CefQuitMessageLoop();
        emit allClosed();
    }
};
#define  qCefCoreAppPrivate() \
QCefCoreApp::getInstance()->d_func()

#endif 

