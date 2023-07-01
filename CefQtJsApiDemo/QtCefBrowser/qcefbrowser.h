﻿#pragma once

#include "qcefbrowserglobal.h"
#include <Qtcore>
#include <QString>

class QCefBrowserPrivate;
class QCEFBROWSER_DECL_EXPORT QCefBrowser : public QObject
{
    friend class QCefBrowserPrivate;
    Q_OBJECT;
public:
    QCefBrowser(QString url);
    inline std::tr1::shared_ptr<QCefBrowserPrivate> d_func()
    {
        return d_ptr;
    }
    virtual ~QCefBrowser();
    void close();
    QString url() const;
    int getId() const;
    int getBrowserId();
private:
    std::tr1::shared_ptr<QCefBrowserPrivate> d_ptr;
};
