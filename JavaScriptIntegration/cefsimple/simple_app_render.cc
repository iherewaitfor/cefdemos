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
        if (name == "myfunc") {
            // Return my string value.
            retval = CefV8Value::CreateString("My Value!");
            return true;
        }

        // Function does not exist.
        return false;
    }
    IMPLEMENT_REFCOUNTING(MyCefV8Handler);
};

class MyV8Accessor : public CefV8Accessor {
public:
    MyV8Accessor() { }

    virtual bool Get(const CefString& name,
        const CefRefPtr<CefV8Value> object,
        CefRefPtr<CefV8Value>& retval,
        CefString& exception) override {
        if (name == "myval") {
            // Return the value.
            retval = CefV8Value::CreateString(myval_);
            return true;
        }

        // Value does not exist.
        return false;
    }

    virtual bool Set(const CefString& name,
        const CefRefPtr<CefV8Value> object,
        const CefRefPtr<CefV8Value> value,
        CefString& exception) override {
        if (name == "myval") {
            if (value->IsString()) {
                // Store the value.
                myval_ = value->GetStringValue();
            }
            else {
                // Throw an exception.
                exception = "Invalid value type";
            }
            return true;
        }

        // Value does not exist.
        return false;
    }

    // Variable used for storing the value.
    CefString myval_;

    // Provide the reference counting implementation for this class.
    IMPLEMENT_REFCOUNTING(MyV8Accessor);
};

void SimpleAppRender::OnWebKitInitialized() {
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
    CefRegisterExtension("v8/test", extensionCode, nullptr);
}
void SimpleAppRender::OnContextCreated(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefV8Context> context) {
    // Retrieve the context's window object.
    CefRefPtr<CefV8Value> window = context->GetGlobal();

    // Create a new V8 string value. See the "Basic JS Types" section below.
    CefRefPtr<CefV8Value> str = CefV8Value::CreateString("My Value in window!");

    // Add the string to the window object as "window.myval". See the "JS Objects" section below.
    window->SetValue("myval", str, V8_PROPERTY_ATTRIBUTE_NONE);


    CefRefPtr<CefV8Accessor> accessor = new MyV8Accessor();
    CefRefPtr<CefV8Value> obj = CefV8Value::CreateObject(accessor, nullptr);
    
    //In order for a value to be passed to the accessor 
    //it must be set using the SetValue() method variant that accepts AccessControl and PropertyAttribute arguments.
    obj->SetValue("myval", V8_ACCESS_CONTROL_DEFAULT,
        V8_PROPERTY_ATTRIBUTE_NONE);
    //¸øobjectÌí¼ÓÊôÐÔ
    obj->SetValue("myval", CefV8Value::CreateString("My Value in myObject!"), V8_PROPERTY_ATTRIBUTE_NONE);
    CefRefPtr<CefV8Handler> handler = new MyCefV8Handler();
    // Create the "myfunc" function.
    CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("myfunc", handler);

    // Add the "myfunc" function to the "window" object.
    obj->SetValue("myfunc", func, V8_PROPERTY_ATTRIBUTE_NONE);

    window->SetValue("myObjcet", obj, V8_PROPERTY_ATTRIBUTE_NONE);
}