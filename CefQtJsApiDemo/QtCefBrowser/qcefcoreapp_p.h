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
public:

    QCefCoreAppPrivate(QCefCoreApp* q)
        : q_ptr(q)
    {

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

        if (m_browsers.empty())
        {
            // to do quit
            CefQuitMessageLoop();
            emit allClosed();
        }
    }
};
#define  qCefCoreAppPrivate() \
QCefCoreApp::getInstance()->d_func()

#endif 

