#pragma once

#include "include/cef_base.h"
#include "include/cef_values.h"
#include "include/cef_browser.h"
#include "include/cef_load_handler.h"
#include "include/cef_v8.h"
#include "include/cef_menu_model.h"
#include "include/cef_render_handler.h"
#include "include/cef_dialog_handler.h"
#include "qcefbrowserglobal.h"
#include <vector>

namespace client {

	class QCEFBROWSER_DECL_EXPORT BrowserDelegate
	{
	public:
		// Called when the browser is created.
		virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser) {}

		// Called when the browser is closing.
		virtual void OnBrowserClosing(CefRefPtr<CefBrowser> browser) {}

		// Called when the browser has been closed.
		virtual void OnBrowserClosed(CefRefPtr<CefBrowser> browser) {}

		virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefLoadHandler::ErrorCode errorCode,
			const CefString& errorText,
			const CefString& failedUrl){};

		virtual void OnLoadStart(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefLoadHandler::TransitionType transition_type){};

		virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			int httpStatusCode){};
        virtual void OnTooltip(CefRefPtr<CefBrowser> browser, const CefString& text){}

		virtual bool OnProcessMessageReceived(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefProcessId source_process,
			CefRefPtr<CefProcessMessage> message)
		{
			return false;
		}

	protected:
		virtual ~BrowserDelegate() {}
	};
}  // namespace client



