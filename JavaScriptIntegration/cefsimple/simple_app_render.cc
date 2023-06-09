#include "simple_app_render.h"
#include <string>

#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_helpers.h"



SimpleAppRender::SimpleAppRender() {}

class MyCefV8Handler : public CefV8Handler {
public:
    ///
    // Handle execution of the function identified by |name|. |object| is the
    // receiver ('this' object) of the function. |arguments| is the list of
    // arguments passed to the function. If execution succeeds set |retval| to the
    // function return value. If execution fails set |exception| to the exception
    // that will be thrown. Return true if execution was handled.
    ///
    /*--cef()--*/
    virtual bool Execute(const CefString& name,
        CefRefPtr<CefV8Value> object,
        const CefV8ValueList& arguments,
        CefRefPtr<CefV8Value>& retval,
        CefString& exception) {
        
        return true;
    }
    IMPLEMENT_REFCOUNTING(MyCefV8Handler);
};
void SimpleAppRender::OnWebKitInitialized() {
    CefRefPtr<CefV8Handler> handler = new MyCefV8Handler();
    ::MessageBox(0, 0, 0, 0);
    // Define the extension contents.
    std::string extensionCode =
        "var test;"
        "if (!test)"
        "  test = {};"
        "(function() {"
        "  test.myval = 'My Value!';"
        "})();";

    // Register the extension.
    CefRegisterExtension("v8/test", extensionCode, handler);
}