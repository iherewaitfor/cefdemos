#include "window.h"
#include "qcefbrowser.h"
#include "qcefcoreapp.h"

QVariantMap GetWindowInfo(QCefBrowser* browser)
{
    QVariantMap map;
    map["windowId"] = browser->getId();
    //map["windowHandle"] = (unsigned int)browser->window()->winId();
    map["cefId"] = browser->getBrowserId();
    return map;
}

namespace CefApi
{
    static const char KWebApiLog[] = "WebApi";



    Window::Window(QObject* parent, QPointer<QCefBrowser> qCefBrowser)
        : QObject(parent)
        , m_browser(qCefBrowser)
    {
        setObjectName("CefWindow");
    }

    Window::~Window()
    {
    }

    Q_INVOKABLE void Window::show()
    {
        if (!m_browser)
            return;
        m_browser->show();
    }

    Q_INVOKABLE void Window::hide()
    {
        if (!m_browser)
            return;
        m_browser->hide();
    }

    Q_INVOKABLE void Window::close()
    {
        if (!m_browser)
            return;
        m_browser->close();
    }

    Q_INVOKABLE void Window::activate()
    {
        if (!m_browser)
            return;
        m_browser->show();
    }
}