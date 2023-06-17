#include <windows.h>
#include "main_util.h"
#include "include/cef_app.h"
#include "qcefrenderapp.h"
#include "qcefotherapp.h"

using namespace shared;
int WINAPI wWinMain(HINSTANCE hInstance,
                      HINSTANCE hPrevInstance,
                      LPTSTR lpCmdLine,
                      int nCmdShow) {
  UNREFERENCED_PARAMETER(hPrevInstance);
  UNREFERENCED_PARAMETER(lpCmdLine);
  UNREFERENCED_PARAMETER(nCmdShow);
      // Provide CEF with command-line arguments.
  CefMainArgs main_args(hInstance);

  // Create a temporary CommandLine object.
  CefRefPtr<CefCommandLine> command_line = CreateCommandLine(main_args);

  // Create a CefApp of the correct process type.
  CefRefPtr<CefApp> app;
  ProcessType processType = GetProcessType(command_line);
  switch (processType) {
    case PROCESS_TYPE_RENDERER:
      app = new QCefRenderApp();
      break;
    case PROCESS_TYPE_OTHER:
      app = new QCefOtherApp();
      break;
  }

  // CEF applications have multiple sub-processes (render, plugin, GPU, etc)
  // that share the same executable. This function checks the command-line and,
  // if this is a sub-process, executes the appropriate logic.
  int exit_code = CefExecuteProcess(main_args, app, nullptr);
  return exit_code;
}