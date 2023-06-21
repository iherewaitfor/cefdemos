#include "include/cef_app.h"
#include <Windows.h>

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    CefMainArgs main_args(hInstance);

    return CefExecuteProcess(main_args, nullptr, NULL);
}