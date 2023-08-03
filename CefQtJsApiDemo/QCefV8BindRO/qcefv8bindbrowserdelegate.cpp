
#include <QVariant>
#include <QtCore>
#include "qcefv8bindroapp.h"
#include "qcefv8bindroapp_p.h"
#include "qcefv8objecthelper.h"
#include "qcefvalue_qvar_convert.h"
#include "qcefv8bindbrowserdelegate.h"
#include "QtCefBrowser/qcefcoreapp.h"


QCefV8BindBrowserDelegate::QCefV8BindBrowserDelegate()
{
}

QCefV8BindBrowserDelegate::~QCefV8BindBrowserDelegate()
{

}

void QCefV8BindBrowserDelegate::OnBrowserCreated(CefRefPtr<CefBrowser> browser)
{
}

void QCefV8BindBrowserDelegate::OnBrowserClosing(CefRefPtr<CefBrowser> browser) {

}

void QCefV8BindBrowserDelegate::OnBrowserClosed(CefRefPtr<CefBrowser> browser)
{
}