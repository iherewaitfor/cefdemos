#ifndef QCEFCOREAPP_P_H
#define QCEFCOREAPP_P_H
#pragma once
#include "include/cef_app.h"

#include "qcefcoreapp.h"
#include <set>
#include <map>
#include <QtCore>
#include "qcefbrowser.h"
class QCefCoreApp;
class QCefBrowser;
class QCefCoreAppPrivate : public CefBaseRefCounted
{
    IMPLEMENT_REFCOUNTING(QCefCoreAppPrivate);
public:
    QCefCoreApp* q_ptr;
    std::set<QCefBrowser*> m_browsers;
    std::map<int, QSharedPointer<QCefBrowser>> m_popupBrowsers;
    std::list<CefRefPtr<CefBrowser>> m_cefbrowsers;
public:

    QCefCoreAppPrivate(QCefCoreApp* q)
        : q_ptr(q)
    {

    }

    ~QCefCoreAppPrivate()
    {
    }

    void addBrowser(QCefBrowser* v)
    {
        m_browsers.insert(v);
    }

    void removeBrowser(QCefBrowser* v)
    {
        m_browsers.erase(v);
    }

    void addPopupBrowser(QSharedPointer<QCefBrowser> browser)
    {
        m_popupBrowsers.insert(std::pair<int, QSharedPointer<QCefBrowser>>(browser->uniqueWindowId(), browser));
    }

    void removePopupBrowser(int uniqueWindowId)
    {
        m_popupBrowsers.erase(uniqueWindowId);
    }
    void removeBrowser(CefRefPtr<CefBrowser>& browser)
    {
        int id = browser->GetIdentifier();
        std::list<CefRefPtr<CefBrowser>>::iterator it = m_cefbrowsers.begin();
        for (; it != m_cefbrowsers.end(); ++it)
        {
            if ((*it)->GetIdentifier() == id)
            {
                m_cefbrowsers.erase(it);
                break;
            }
        }

        if (m_cefbrowsers.empty())
        {
            // to do quit
        }
    }
};
#define  qCefCoreAppPrivate() \
QCefCoreApp::getInstance()->d_func()

#endif 

