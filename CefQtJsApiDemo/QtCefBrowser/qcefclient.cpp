#include "qcefclient.h"
#include "qcefbrowserapp.h"

#include "render/qcefrenderapp.h"

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
    cefSettings.remote_debugging_port = 8765;
    QString app;

    // get the qcefrender.exe
    wchar_t path[MAX_PATH];
    ::GetModuleFileName(NULL, path, MAX_PATH);
    std::wstring strPath = path;
    QString modulPath = QString::fromWCharArray(path);;
    modulPath.replace("\\", "/");
    modulPath = modulPath.left(modulPath.lastIndexOf("/")+1);

    // QString logpath = modulPath + "a.log";
    // CefString(&cefSettings.log_file) = logpath.toUtf8().constData();
    
    modulPath.append("qcefrender.exe");

    CefString(&cefSettings.browser_subprocess_path) = modulPath.toUtf8().constData();

    CefInitialize(*m_cefMainArgs, cefSettings, m_cefBrowserApp.get(), NULL);
}

void QCefClient::shutDownCef() {
    CefShutdown();
    emit shutdown();
}

int QCefClient::initCefRender(){

    CefMainArgs cefMainArgs(GetModuleHandle(NULL));
    CefRefPtr<CefApp> app;
    CefRefPtr<CefCommandLine>  commandLine = CefCommandLine::CreateCommandLine();
    commandLine->InitFromString(::GetCommandLine());
    if (!commandLine->HasSwitch("type"))
    {
    }
    else
    {
        const std::string& process_type = commandLine->GetSwitchValue("type");
        if (process_type == "renderer")
        {
            app = new QCefRenderApp();
        }
    }
    int exit_code = CefExecuteProcess(cefMainArgs, app, nullptr);
    return exit_code;
    
}