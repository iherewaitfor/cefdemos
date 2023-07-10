#pragma once

#include "qcefbrowserglobal.h"
#include <Qtcore>
#include <QString>
#include "browser/browser_struct.h"
class QCefBrowserPrivate;
class QCEFBROWSER_DECL_EXPORT QCefBrowser : public QObject
{
    friend class QCefBrowserPrivate;
    Q_OBJECT;
public:
    QCefBrowser(const BrowserWindowOptions& option);
    inline std::tr1::shared_ptr<QCefBrowserPrivate> d_func()
    {
        return d_ptr;
    }
    virtual ~QCefBrowser();
    void close();
    QString url() const;
    int getId() const;
    int getBrowserId();

    void show();
    void hide();
private:
    std::tr1::shared_ptr<QCefBrowserPrivate> d_ptr;
};
