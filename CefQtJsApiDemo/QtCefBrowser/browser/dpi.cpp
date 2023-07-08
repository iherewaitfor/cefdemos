#include "dpi.h"
#include <Windows.h>

namespace windowlib {

    static bool g_enbaleHIDPI = false;

    DPI::DPI()
        : defDpi_(96)
    {
        dpiX_ = 96;
        dpiY_ = 96;
        HDC hdc = GetDC(NULL);
        if (hdc)
        {
            dpiX_ = GetDeviceCaps(hdc, LOGPIXELSX);
            dpiY_ = GetDeviceCaps(hdc, LOGPIXELSY);
            ReleaseDC(NULL, hdc);
        }
    }

    DPI* DPI::instance()
    {
        static DPI dpi;
        return &dpi;
    }

    void DPI::enableHIDPI(bool enable)
    {
        g_enbaleHIDPI = enable;

        typedef enum _PROCESS_DPI_AWARENESS {
            PROCESS_DPI_UNAWARE = 0,
            PROCESS_SYSTEM_DPI_AWARE = 1,
            PROCESS_PER_MONITOR_DPI_AWARE = 2
        } PROCESS_DPI_AWARENESS;

        typedef BOOL(WINAPI* PSetProcessDPIAware)(void);
        typedef HRESULT(WINAPI* PSetProcessDpiAwareness)(_In_ PROCESS_DPI_AWARENESS value);

        HMODULE hModule = GetModuleHandleW(L"User32.dll");
        HMODULE hShcore = LoadLibraryW(L"Shcore.dll");

        do
        {
            if (hShcore == NULL && hModule == NULL)
            {
                break;
            }

            PSetProcessDpiAwareness pSetProcessDpiAwareness = NULL;
            PSetProcessDPIAware pSetProcessDPI = NULL;
            if (hShcore != NULL)
            {
                pSetProcessDpiAwareness = (PSetProcessDpiAwareness)GetProcAddress(hShcore, "SetProcessDpiAwareness");
            }

            if (hModule != NULL)
            {
                pSetProcessDPI = (PSetProcessDPIAware)GetProcAddress(hModule, "SetProcessDPIAware");
            }

            if (pSetProcessDpiAwareness != NULL)
            {
                if (enable)
                {
                    pSetProcessDpiAwareness(PROCESS_SYSTEM_DPI_AWARE);

                }
                else
                {
                    pSetProcessDpiAwareness(PROCESS_DPI_UNAWARE);

                }

            }
            else if (pSetProcessDPI != NULL)
            {
                if (enable)
                {
                    pSetProcessDPI();
                }
            }
        } while (FALSE);

        if (hShcore != NULL)
        {
            FreeLibrary(hShcore);
        }
    }

    bool DPI::isHIDPIEnabled()
    {
        return g_enbaleHIDPI;
    }

}
