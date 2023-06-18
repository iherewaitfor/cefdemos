#include "qcefclient.h"
#include "qcefbrowserapp.h"

QCefClient::QCefClient(QObject* parent /*= nullptr*/)
    : QObject(parent)
{
}

QCefClient::~QCefClient()
{

}

void QCefClient::initCef()
{
    m_cefMainArgs.reset(new CefMainArgs(GetModuleHandle(NULL)));
    m_cefBrowserApp = new QCefBrowserApp;

    CefSettings cefSettings;
    cefSettings.multi_threaded_message_loop = true;
    cefSettings.no_sandbox = true;
    QString app;

    // get the qcefrender.exe
    wchar_t path[MAX_PATH];
    ::GetModuleFileName(NULL, path, MAX_PATH);
    QString modulPath = QString::fromStdWString(path);
    modulPath = modulPath.left(modulPath.lastIndexOf("qcefbrowser.dll"));
    modulPath.append("qcefrender.exe");

    CefString(&cefSettings.browser_subprocess_path) = modulPath.toUtf8().constData();

    CefInitialize(*m_cefMainArgs, cefSettings, m_cefBrowserApp.get(), NULL);
}
void QCefClient::shutDownCef() {

}