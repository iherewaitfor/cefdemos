#include <windows.h>

#include "include/cef_app.h"

#include "../QtCefBrowser/qcefclient.h"

int WINAPI wWinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPTSTR lpCmdLine,
                      int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);
  UNREFERENCED_PARAMETER(nCmdShow);
      // Provide CEF with command-line arguments.
  CefMainArgs main_args(hInstance);

  QCefClient cefClient;
  int exitcode = cefClient.initCefRender();
  return exitcode;
}