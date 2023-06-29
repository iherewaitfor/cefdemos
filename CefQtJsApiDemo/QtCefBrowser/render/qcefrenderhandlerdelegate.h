// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#pragma once
#include "include/cef_base.h"
#include "include/cef_values.h"
#include "include/cef_browser.h"
#include "include/cef_v8.h"
#include "../qcefbrowserglobal.h"

namespace client {

	class QCEFBROWSER_DECL_EXPORT RenderDelegate
    {
	public:
		virtual void OnRenderThreadCreated(CefRefPtr<CefListValue> extra_info) {};

		virtual void OnWebKitInitialized() {}

		virtual void OnBrowserCreated(CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefDictionaryValue> extra_info) {}

		virtual void OnBrowserDestroyed(CefRefPtr<CefBrowser> browser) {}


		virtual void OnContextCreated(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefV8Context> context) {}

		virtual void OnContextReleased(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefRefPtr<CefV8Context> context) {}

		virtual bool OnProcessMessageReceived(
			CefRefPtr<CefBrowser> browser,
			CefRefPtr<CefFrame> frame,
			CefProcessId source_process,
			CefRefPtr<CefProcessMessage> message) {
				return false;
		}

        //virtual void OnLoadStart(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int transition_type) OVERRIDE
        //{
        //}

        //virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode) OVERRIDE
        //{
        //}
	};

}  // namespace client



