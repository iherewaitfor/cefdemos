#include "qcefbrowser.h"
#include "qcefbrowser_p.h"
#include "qcefcoreapp.h"
QCefBrowser::QCefBrowser(QString url)
    : QObject(NULL)
    , d_ptr(new QCefBrowserPrivate(this, url))
{
    BrowserWindowOptions options;
    options.url = url.toStdString();
    options.width = 500;
    options.height = 500;
    options.topMost = true;
    options.visible = true;

    d_ptr->initWindowOptions(options);
    d_ptr->createHostWindow();
    d_ptr->createBrowser();
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