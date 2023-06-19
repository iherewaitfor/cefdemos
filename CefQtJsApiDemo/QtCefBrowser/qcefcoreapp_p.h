#ifndef QCEFCOREAPP_P_H
#define QCEFCOREAPP_P_H
#pragma once
#include "include/cef_app.h"

#include "qcefcoreapp.h"

class QCefCoreApp;
class YYCefBrowser;
class QCefCoreAppPrivate : public CefBaseRefCounted
{
    IMPLEMENT_REFCOUNTING(QCefCoreAppPrivate);
public:
    QCefCoreApp* q_ptr;

public:

    QCefCoreAppPrivate(QCefCoreApp* q)
        : q_ptr(q)
    {

    }

    ~QCefCoreAppPrivate()
    {
    }
};
#define  qCefCoreAppPrivate() \
YYCefCoreApp::getInstance()->d_func()

#endif 

