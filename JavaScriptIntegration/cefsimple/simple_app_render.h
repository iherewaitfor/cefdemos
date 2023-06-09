#ifndef SIMPLEAPPRENDER_H
#define SIMPLEAPPRENDER_H

#include "include/cef_app.h"

// Implement application-level callbacks for the browser process.
class SimpleAppRender : public CefApp, public CefRenderProcessHandler {
 public:
     SimpleAppRender();

  // CefApp methods:
  virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() {
      return this;
  }

  // CefRenderProcessHandler methods:
  virtual void OnWebKitInitialized();

  virtual void OnContextCreated(CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      CefRefPtr<CefV8Context> context) {}
  virtual void OnContextReleased(CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      CefRefPtr<CefV8Context> context) {}
 private:
  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(SimpleAppRender);
};

#endif  // SIMPLEAPPRENDER_H
