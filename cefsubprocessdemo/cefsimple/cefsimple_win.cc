#include "include/cef_app.h"
#include "include/cef_browser.h"
#include "include/cef_client.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"
#include <Windows.h>

class MyClient : public CefClient, public CefLifeSpanHandler
{
public:
    virtual ~MyClient() {}
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() override { return this; }
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) override { CefQuitMessageLoop(); }
private:
    IMPLEMENT_REFCOUNTING(MyClient);
};

class MyApp : public CefApp, public CefBrowserProcessHandler
{
public:
    virtual ~MyApp() {}
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override { return this; }
    virtual void OnContextInitialized() override
    {
        CEF_REQUIRE_UI_THREAD();
        CefWindowInfo window_info;
        window_info.SetAsPopup(NULL, "cefsimple");
        CefRefPtr<MyClient> client(new MyClient());
        CefString url = "http://www.baidu.com";
        CefBrowserSettings browser_settings;
        CefBrowserHost::CreateBrowser(window_info, client, url, browser_settings, nullptr,nullptr);
    }
private:
    IMPLEMENT_REFCOUNTING(MyApp);
};

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    CefMainArgs main_args(hInstance);
    CefSettings settings;
    settings.no_sandbox = true;
    // 获取子进程路径
    WCHAR subProcessPath[MAX_PATH] = { 0 };
    GetModuleFileNameW(hInstance, subProcessPath, MAX_PATH);
    *(wcsrchr(subProcessPath, L'\\') + 1) = L'\0';
    LPCWSTR SUB_PROCESS_NAME = L"cefrender.exe";
    wcsncat_s(subProcessPath, MAX_PATH, SUB_PROCESS_NAME, wcslen(SUB_PROCESS_NAME));
    // 设置子进程路径
    cef_string_from_wide(subProcessPath, MAX_PATH, &settings.browser_subprocess_path);

    auto myApp = CefRefPtr<MyApp>(new MyApp());

    CefInitialize(main_args, settings, myApp.get(), NULL);
    CefRunMessageLoop();
    CefShutdown();

    return 0;
}