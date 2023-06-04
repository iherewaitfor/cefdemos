// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_
#define CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_

#include "include/cef_app.h"
#include <QObject>
#include <QTimer>

// Implement application-level callbacks for the browser process.
class SimpleApp :public QObject,public CefApp, public CefBrowserProcessHandler{
    Q_OBJECT
 public:
  SimpleApp(QObject* parent = nullptr);

  // CefApp methods:
  CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() override {
    return this;
  }

  // CefBrowserProcessHandler methods:
  void OnContextInitialized() override;
  CefRefPtr<CefClient> GetDefaultClient() override;
public slots:
    void delayOpenBrowserTest();
 private:
  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(SimpleApp);
  QTimer timer;
};

#endif  // CEF_TESTS_CEFSIMPLE_SIMPLE_APP_H_
