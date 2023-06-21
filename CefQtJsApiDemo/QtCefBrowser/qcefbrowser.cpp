#include "qcefbrowser.h"
#include "qcefbrowser_p.h"
#include "qcefcoreapp.h"
QCefBrowser::QCefBrowser(QString url)
    : QObject(NULL)
    , d_ptr(new QCefBrowserPrivate(this, url))
{
    d_ptr->createBrowser();
    qCefCoreAppPrivate()->addBrowser(this);
}

QCefBrowser::~QCefBrowser()
{
    
    qCefCoreAppPrivate()->removeBrowser(this);

    d_ptr->q_ptr = NULL;
}

void QCefBrowser::close()
{
    d_ptr->closeBroser();
}

QString QCefBrowser::url() const
{
    return d_ptr->m_url;
}
int QCefBrowser::uniqueWindowId() const {
    return d_ptr->m_uniqueWindowId;
}