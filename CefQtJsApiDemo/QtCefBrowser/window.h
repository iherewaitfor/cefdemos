#pragma once
#include <QtCore>
#include "qcefbrowserglobal.h"
class QCefBrowser;

namespace CefApi
{
class QCEFBROWSER_DECL_EXPORT Window
    : public QObject
{
    Q_OBJECT;
public:
    Window(QObject* parent, QPointer<QCefBrowser> qCefBrowser);
    virtual ~Window();

    Q_INVOKABLE void show();
    Q_INVOKABLE void hide();
    Q_INVOKABLE void close();
    Q_INVOKABLE void activate();

private:
    QPointer<QCefBrowser> m_browser;
};
} // namespace CefApi
