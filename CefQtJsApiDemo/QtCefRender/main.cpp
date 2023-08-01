#include <windows.h>

#include "include/cef_app.h"

#include "../QtCefBrowser/qcefclient.h"
//#include "../QCefV8Bind/qcefv8bindapp.h"
#include "../QCefV8BindRO/qcefv8bindroapp.h"
#include <QtCore>
#include <process.h>
unsigned int __stdcall ThreaCefRender(PVOID pM);
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
  bool brender = false;
  if(commandLine->HasSwitch("type"))
  {
      const std::string& process_type = commandLine->GetSwitchValue("type");
      if (process_type == "renderer")
      {
		brender = true;
        //#define RENDER_DEUGB
        #ifdef   RENDER_DEUGB
                  char szBuf[MAX_PATH] = { 0 };
                  sprintf(szBuf, "C:\\Windows\\System32\\VSJitDebugger.exe -p %d", GetCurrentProcessId());
                  WinExec(szBuf, SW_SHOW);
        #endif
      }
  }
  int exitcode = 0;
  if (brender) {
      int argc = 0;
      char* argv[] = { "abc", "def" };
      QCoreApplication qApplication(argc, argv);
      //QCefV8BindApp::getInstance();
      QCefV8BindAppRO::getInstance();
      unsigned threadID = 0;
      HANDLE tHandle = NULL;
      tHandle = (HANDLE)_beginthreadex(NULL, 0, &ThreaCefRender, NULL, 0, &threadID);
      exitcode = qApplication.exec();
  }
  else {
      QCefClient cefClient;
      exitcode = cefClient.initCefRender();
  }
  return exitcode;
}

unsigned int __stdcall ThreaCefRender(PVOID pM) {
    QCefClient cefClient;
    int exitcode = cefClient.initCefRender();
    QCoreApplication::quit();
    return exitcode;
}