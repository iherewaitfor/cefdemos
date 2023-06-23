#include "qcefbrowser.h"
#include "qcefbrowser_p.h"
#include "qcefcoreapp.h"
QCefBrowser::QCefBrowser(QString url)
    : QObject(NULL)
    , d_ptr(new QCefBrowserPrivate(this, url))
{
    d_ptr->createBrowser();
    qCefCoreAppPrivate()->addBrowser(QPointer<QCefBrowser>(this));
}

QCefBrowser::~QCefBrowser()
{
    
    qCefCoreAppPrivate()->removeBrowser(QPointer<QCefBrowser>(this));

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
int QCefBrowser::uniqueWindowId() const {
    return d_ptr->m_uniqueWindowId;
}