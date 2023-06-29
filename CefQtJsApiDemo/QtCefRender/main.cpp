#include <windows.h>

#include "include/cef_app.h"

#include "../QtCefBrowser/qcefclient.h"
#include "../QCefV8Bind/qcefv8bindapp.h"

int WINAPI wWinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPTSTR lpCmdLine,
                      int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);
  UNREFERENCED_PARAMETER(nCmdShow);
      // Provide CEF with command-line arguments.
  CefMainArgs main_args(hInstance);
  CefRefPtr<CefCommandLine>  commandLine = CefCommandLine::CreateCommandLine();
  commandLine->InitFromString(::GetCommandLine());
  if (commandLine->HasSwitch("type"))
  {
      const std::string& process_type = commandLine->GetSwitchValue("type");
      if (process_type == "renderer")
      {
        //#define VSDEUGB
        #ifdef   VSDEUGB
                  char szBuf[MAX_PATH] = { 0 };
                  sprintf(szBuf, "C:\\Windows\\System32\\VSJitDebugger.exe -p %d", GetCurrentProcessId());
                  WinExec(szBuf, SW_SHOW);
        #endif
      }
  }

  QCefV8BindApp::getInstance();
  QCefClient cefClient;
  int exitcode = cefClient.initCefRender();
  return exitcode;
}