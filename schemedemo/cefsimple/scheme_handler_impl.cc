// Copyright (c) 2017 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "scheme_handler_impl.h"

#include "include/cef_browser.h"
#include "include/cef_callback.h"
#include "include/cef_frame.h"
#include "include/cef_request.h"
#include "include/cef_resource_handler.h"
#include "include/cef_response.h"
#include "include/cef_scheme.h"
#include "include/wrapper/cef_helpers.h"

#include "scheme_strings.h"

namespace scheme_handler {

namespace {
    // help method
    std::wstring getResourceRootPath() {
        static std::wstring strPath;
        if (strPath.empty()) {
            TCHAR path[MAX_PATH];
            ::GetModuleFileName(NULL, path, MAX_PATH);
            strPath = path;

            size_t index = strPath.rfind(TEXT("\\"));
            strPath = strPath.substr(0, index + 1);
        }
        return strPath;
    }
    // help method
    bool readFromFile(std::wstring strRelativePath, std::string &dataDst) {
        FILE* infile = NULL;
        static char* buf = NULL; //
        std::wstring strPath = getResourceRootPath() + strRelativePath;
        if ((infile = _wfopen(strPath.c_str(), L"rb")) == NULL) {
            printf("cannot open this file\n");
            return  false;
        }
        if (buf == NULL) {
            buf = new  char[1000000];
        }
        // for convinient, the test file is less then 1000000
        ZeroMemory(buf, 1000000);
        size_t rd = fread(buf, 1, 1000000, infile);
        if (rd > 0) {
            dataDst.resize(rd);
            dataDst.assign(buf, rd);
        }
        if (infile) {
            fclose(infile);
            infile = NULL;
        }
        return true;
    }
// Implementation of the scheme handler for client:// requests.
class ClientSchemeHandler : public CefResourceHandler {
 public:
  ClientSchemeHandler() : offset_(0) {}

  bool ProcessRequest(CefRefPtr<CefRequest> request,
                      CefRefPtr<CefCallback> callback) override {
    CEF_REQUIRE_IO_THREAD();
    return false;
  }

  bool Open(CefRefPtr<CefRequest> request,
      bool& handle_request,
      CefRefPtr<CefCallback> callback) {
      handle_request = false;

      std::string url = request->GetURL();
      if (strstr(url.c_str(), kFileName) != nullptr) {
          // Load the response html.
          std::wstring relativePath = L"scheme_test.html";
          if (!readFromFile(relativePath, data_)) {
              return false;
          }
          handle_request = true;
          mime_type_ = "text/html";
      }
      else if (strstr(url.c_str(), "logo.png") != nullptr) {
          // Load the response image.
          std::wstring relativePath = L"logo.png";
          if (!readFromFile(relativePath, data_)) {
              return false;
          }
          handle_request = true;
          mime_type_ = "image/png";
      }

      if (handle_request) {
          // Indicate that the headers are available.
          callback->Continue();
          return true;
      }
      return false;
  }
  void GetResponseHeaders(CefRefPtr<CefResponse> response,
                          int64& response_length,
                          CefString& redirectUrl) override {
    CEF_REQUIRE_IO_THREAD();

    DCHECK(!data_.empty());

    response->SetMimeType(mime_type_);
    response->SetStatus(200);

    // Set the resulting response length.
    response_length = data_.length();
  }

  void Cancel() override { CEF_REQUIRE_IO_THREAD(); }

  bool ReadResponse(void* data_out,
                    int bytes_to_read,
                    int& bytes_read,
                    CefRefPtr<CefCallback> callback) override {
    CEF_REQUIRE_IO_THREAD();

    bool has_data = false;
    bytes_read = 0;

    if (offset_ < data_.length()) {
      // Copy the next block of data into the buffer.
      int transfer_size =
          std::min(bytes_to_read, static_cast<int>(data_.length() - offset_));
      memcpy(data_out, data_.c_str() + offset_, transfer_size);
      offset_ += transfer_size;

      bytes_read = transfer_size;
      has_data = true;
    }

    return has_data;
  }

 private:
  std::string data_;
  std::string mime_type_;
  size_t offset_;

  IMPLEMENT_REFCOUNTING(ClientSchemeHandler);
  DISALLOW_COPY_AND_ASSIGN(ClientSchemeHandler);
};

// Implementation of the factory for creating scheme handlers.
class ClientSchemeHandlerFactory : public CefSchemeHandlerFactory {
 public:
  ClientSchemeHandlerFactory() {}

  // Return a new scheme handler instance to handle the request.
  CefRefPtr<CefResourceHandler> Create(CefRefPtr<CefBrowser> browser,
                                       CefRefPtr<CefFrame> frame,
                                       const CefString& scheme_name,
                                       CefRefPtr<CefRequest> request) override {
    CEF_REQUIRE_IO_THREAD();
    return new ClientSchemeHandler();
  }

 private:
  IMPLEMENT_REFCOUNTING(ClientSchemeHandlerFactory);
  DISALLOW_COPY_AND_ASSIGN(ClientSchemeHandlerFactory);
};

}  // namespace

void RegisterSchemeHandlerFactory() {
  CefRegisterSchemeHandlerFactory(kScheme, kDomain,
                                  new ClientSchemeHandlerFactory());
}

}  // namespace scheme_handler
