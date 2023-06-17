#pragma once
#include "include/cef_app.h"
#include <set>

class QCefOtherApp : public CefApp
{
public:
    QCefOtherApp();

private:

    IMPLEMENT_REFCOUNTING(QCefOtherApp);
    DISALLOW_COPY_AND_ASSIGN(QCefOtherApp);
    
};  

