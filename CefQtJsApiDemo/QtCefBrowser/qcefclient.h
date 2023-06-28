#ifndef QCEFCLIENT_H
#define QCEFCLIENT_H
#include "qcefbrowserglobal.h"
#include <QtCore>
#include "include/cef_app.h"
class QCefBrowserApp;

class QCEFBROWSER_DECL_EXPORT QCefClient : public QObject
{
    Q_OBJECT;
public:
    explicit QCefClient(QObject* parent = nullptr);
    ~QCefClient();
    void initCef();
    int initCefRender();
private slots:
    void shutDownCef();
signals:
    void shutdown();
private:
    CefRefPtr<QCefBrowserApp> m_cefBrowserApp;
    std::unique_ptr<CefMainArgs> m_cefMainArgs;
};

#endif

