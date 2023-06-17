#include "main_util.h"
#include <windows.h>

namespace shared {
    namespace {

        // These flags must match the Chromium values.
        const char kProcessType[] = "type";
        const char kRendererProcess[] = "renderer";

    }  // namespace

    CefRefPtr<CefCommandLine> CreateCommandLine(const CefMainArgs& main_args) {
        CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
        command_line->InitFromString(::GetCommandLineW());
        return command_line;
    }

    ProcessType GetProcessType(const CefRefPtr<CefCommandLine>& command_line) {
        // The command-line flag won't be specified for the browser process.
        if (!command_line->HasSwitch(kProcessType))
            return PROCESS_TYPE_BROWSER;

        const std::string& process_type = command_line->GetSwitchValue(kProcessType);
        if (process_type == kRendererProcess)
            return PROCESS_TYPE_RENDERER;

        return PROCESS_TYPE_OTHER;
    }
}

