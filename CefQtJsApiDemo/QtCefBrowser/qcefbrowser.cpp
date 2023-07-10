#include "qcefbrowser.h"
#include "qcefbrowser_p.h"
#include "qcefcoreapp.h"
QCefBrowser::QCefBrowser(const BrowserWindowOptions& option)
    : QObject(NULL)
    , d_ptr(new QCefBrowserPrivate(this))
{
    //BrowserWindowOptions options;
    //options.url = url.toStdString();
    //options.width = 1200;
    //options.height = 900;
    //options.minWidth = 300;
    //options.minHeight = 300;
    //options.showInTaskbar = true;
    //options.topMost = true;
    //options.visible = true;
    //options.frameless = false;
    //std::vector<RECT> capVector;
    //RECT rect = { 0, 0, 1200, 100 };
    //capVector.push_back(rect);
    //options.captionAreas = capVector;

    d_ptr->initWindowOptions(option);
    d_ptr->initWindow(); // crate browser in TID_UI thread.

    d_ptr->showWindowTID_UI(d_ptr->m_windowOptions.visible.value());
}

QCefBrowser::~QCefBrowser()
{
    d_ptr->q_ptr = NULL;
}

void QCefBrowser::close()
{
    d_ptr->closeBrowser();
}

QString QCefBrowser::url() const
{
    return d_ptr->m_url;
}
int QCefBrowser::getId() const {
    return d_ptr->m_Id;
}
int QCefBrowser::getBrowserId() {
    return d_ptr->m_browserId;
}

void QCefBrowser::show()
{
    d_ptr->showWindowTID_UI(true);
}

void QCefBrowser::hide()
{
    d_ptr->showWindowTID_UI(false);
}
