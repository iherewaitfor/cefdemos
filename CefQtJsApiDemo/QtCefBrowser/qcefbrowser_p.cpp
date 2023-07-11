
#include "qcefbrowser_p.h"
#include "qcefcoreapp.h"
#include "qcefcoreapp_p.h"

#include "include/cef_parser.h"
#include <include/cef_app.h>
#include <include/cef_browser.h>
#include <QtCore>
#include "include/wrapper/cef_helpers.h"

static int win_id_generator = 0;
QCefBrowserPrivate::QCefBrowserPrivate(QCefBrowser* q)
    : q_ptr(q)
    , QObject(nullptr)
    , m_parent(NULL)
    , m_closing(0)
    , m_browserId(0)
{
    m_Id = ++win_id_generator;

    connect(this, SIGNAL(afterCreated(int)), SLOT(OnAfterCreatedSlot(int)));
    connect(this, SIGNAL(beforeClose(int)), SLOT(OnBeforeCloseSlot(int)));
    connect(this, SIGNAL(closing(int)), SLOT(OnClosingSlot(int)));

    connect(this, SIGNAL(afterCreatedPoppup(int)), SLOT(afterCreatedPoppupSlot(int)));
    connect(this, SIGNAL(beforeClosePoppup(int)), SLOT(beforeClosePoppupSlot(int)));
}

QCefBrowserPrivate::~QCefBrowserPrivate()
{
}

void QCefBrowserPrivate::onHostWindowWillClose()
{
    if (modalMode() && m_parent)
    {
        EnableWindow(m_parent, true);
    }
}

void QCefBrowserPrivate::onHostWindowDestroy()
{
    closeBrowser();

    //if (q_ptr)
    //    q_ptr->windowDestroyed(m_uniqueWindowId);
}

bool QCefBrowserPrivate::modalMode() const
{
    return m_windowOptions.modalMode.value();
}
MONITORINFO GetMonitorInformation(HWND handle)
{
    MONITORINFO mi;
    mi.cbSize = sizeof(mi);
    HMONITOR monitor = MonitorFromWindow(handle, MONITOR_DEFAULTTONEAREST);
    GetMonitorInfo(monitor, &mi);
    return mi;
}

void QCefBrowserPrivate::createHostWindow()
{
    m_hostWindow = new QCefBrowserWindow(this);

    m_hostWindow->setBackgourndColor(RGB(255, 255, 255));
    m_hostWindow->setFrameless(m_windowOptions.frameless.value(),false);
    m_hostWindow->setResizable(m_windowOptions.resizable.value());
    m_hostWindow->setShowInTaskbar(m_windowOptions.showInTaskbar.value());
    m_hostWindow->setNcBorderWidth(m_windowOptions.ncBorderWidth.value());

    if (m_windowOptions.maxWidth.has_value())
        m_hostWindow->setMaxWidth(m_windowOptions.maxWidth.value());
    if (m_windowOptions.maxHeight.has_value())
        m_hostWindow->setMaxHeight(m_windowOptions.maxHeight.value());
    if (m_windowOptions.minWidth.has_value())
        m_hostWindow->setMinWidth(m_windowOptions.minWidth.value());
    if (m_windowOptions.minHeight.has_value())
        m_hostWindow->setMinHeight(m_windowOptions.minHeight.value());

    if (m_windowOptions.lifeGuard.value())
        m_hostWindow->SetAppLifeGuard();

    // create window
    if (m_parent && m_windowOptions.asChild.value())
    {
        m_hostWindow->createAsChild(m_parent);
    }
    else
    {
        if (m_parent)
        {
            MONITORINFO info = GetMonitorInformation(m_parent);
            int workWidth = info.rcWork.right - info.rcWork.left;
            int workHeight = info.rcWork.bottom - info.rcWork.top;
            POINT p = { 0 };
            p.x = info.rcWork.left + (workWidth - (m_hostWindow->width())) / 2;
            p.y = info.rcWork.top + (workHeight - (m_hostWindow->height())) / 2;

            if (p.x < info.rcWork.left)
            {
                p.x = info.rcWork.left;
            }
            if (p.y < info.rcWork.top)
            {
                p.y = info.rcWork.top;
            }
            m_hostWindow->move(p);
        }
        else
        {
            POINT pt = {};
            GetCursorPos(&pt);
            pt.x -= m_hostWindow->width() / 2;
            pt.y -= m_hostWindow->height() / 2;
            m_hostWindow->move(pt);
        }

        m_hostWindow->create(m_parent, 0,0);
    }

    SIZE sz = { m_windowOptions.width.value(), m_windowOptions.height.value() };
    m_hostWindow->resize(sz);


    if (m_windowOptions.x.has_value() || m_windowOptions.y.has_value())
    {
        POINT pt = { m_windowOptions.x.value_or(m_hostWindow->pos().x),
                     m_windowOptions.y.value_or(m_hostWindow->pos().y) };
        m_hostWindow->move(pt);
    }
    else
    {
        m_hostWindow->moveCenter();
    }

    if (modalMode() && m_parent)
    {
        EnableWindow(m_parent, false);
    }

    if (m_windowOptions.title.has_value())
    {
        m_hostWindow->SetTitle(QString::fromUtf8(m_windowOptions.title.value().c_str()).toStdWString());
    }

    if (m_windowOptions.captionAreas.has_value())
    {
        m_hostWindow->setCaptionArea(m_windowOptions.captionAreas.value());
    }
}



void QCefBrowserPrivate::createBrowser() {
    //CefRefPtr<SimpleHandler> clientHandler = new SimpleHandler(shared_from_this());
    //m_clientHandler = clientHandler;

    CefWindowInfo windowInfo;
    CefBrowserSettings settings;

    //windowInfo.SetAsPopup(NULL, "test");

    RECT wnd_rect = m_hostWindow->clientRect();
    CefRect rect(wnd_rect.left, wnd_rect.top, wnd_rect.right - wnd_rect.left, wnd_rect.bottom - wnd_rect.top);
    windowInfo.SetAsChild(m_hostWindow->winId(), rect);

    CefString url = m_url.toUtf8().constData();
    CefBrowserHost::CreateBrowser(windowInfo, m_clientHandler, url, settings, nullptr, nullptr);
}

void QCefBrowserPrivate::closeBrowser()
{
    base::AutoLock scopLock(lock);
    if(!m_clientHandler || m_closing != 0)
    if (m_clientHandler) {
        m_clientHandler->closeMainBrowser();
    }
}
void QCefBrowserPrivate::initWindowOptions(const BrowserWindowOptions& options)
{
    m_clientHandler = new SimpleHandler(shared_from_this());
    BrowserWindowOptions windowOptions = options;
    if (!windowOptions.visible.has_value())
        windowOptions.visible = true;
    if (!windowOptions.topMost.has_value())
        windowOptions.topMost = false;
    if (!windowOptions.offscreen.has_value())
        windowOptions.offscreen = false;
    if (!windowOptions.transparent.has_value())
        windowOptions.transparent = false;
    if (!windowOptions.lifeGuard.has_value())
        windowOptions.lifeGuard = false;
    if (!windowOptions.ignoreSysClose.has_value())
        windowOptions.ignoreSysClose = false;
    if (!windowOptions.showInTaskbar.has_value())
        windowOptions.showInTaskbar = true;

    if (!windowOptions.ncBorderWidth.has_value())
        windowOptions.ncBorderWidth = 8;
    if (!windowOptions.width.has_value())
        windowOptions.width = 400;
    if (!windowOptions.height.has_value())
        windowOptions.height = 300;
    if (!windowOptions.resizable.has_value())
        windowOptions.resizable = true;

    if (!windowOptions.frameless.has_value())
        windowOptions.frameless = true;
    if (windowOptions.offscreen.value())
        windowOptions.frameless = true;

    if (!windowOptions.parentHandle.has_value())
        windowOptions.parentHandle = 0;
    m_parent = (HWND)(windowOptions.parentHandle.value());

    if (!m_windowOptions.asChild.has_value())
        m_windowOptions.asChild = false;

    if (!windowOptions.modalMode.has_value())
        windowOptions.modalMode = false;
    windowOptions.osrMode = false;


    m_windowOptions = windowOptions;
    if (m_windowOptions.url.has_value())
    {
        m_startUpUrl = QString::fromUtf8(m_windowOptions.url.value().c_str());
        m_url = m_startUpUrl;
    }
}

void QCefBrowserPrivate::setSize()
{
    if (!m_hostWindow)
    {
        return;
    }

    base::AutoLock scopLock(lock);
    if (m_clientHandler) {
        m_clientHandler->setSize();
    }
}
void QCefBrowserPrivate::initWindow() {
    base::AutoLock scopLock(lock);
    if (m_clientHandler) {
        m_clientHandler->initWindow();
    }
}
BOOL WINAPI hookChildFunc(HWND hWnd, LPARAM lParam)
{
    CEF_REQUIRE_UI_THREAD();
    QCefBrowserPrivate* pThis = (QCefBrowserPrivate*)lParam;
    if (!pThis->findHookedChildWindow(hWnd))
    {
        QSharedPointer<QCefBrowserChildWindow> hook =
            QSharedPointer<QCefBrowserChildWindow>(new QCefBrowserChildWindow(hWnd, pThis));
        pThis->m_hookChildWindows.push_back(hook);
    }
    return TRUE;
};
void QCefBrowserPrivate::hookChildWindows()
{
    CEF_REQUIRE_UI_THREAD();
    if (m_clientHandler) {
        HWND window = m_clientHandler->m_browser->GetHost()->GetWindowHandle();
        hookChildFunc(window, (LONG_PTR)this);
        EnumChildWindows(window, hookChildFunc, (LONG_PTR)this);
    }
}

bool QCefBrowserPrivate::findHookedChildWindow(HWND window)
{
    CEF_REQUIRE_UI_THREAD();
    std::vector<QSharedPointer<QCefBrowserChildWindow>>::iterator it = m_hookChildWindows.begin();
    for (; it != m_hookChildWindows.end(); ++it)
    {
        if ((*it)->winId() == window)
        {
            return true;
        }
    }
    return false;
}

void QCefBrowserPrivate::showWindowTID_UI(bool visible)
{
    base::AutoLock scopLock(lock);
    //切到TID_UI线程执行。
    if (m_clientHandler) {
        m_clientHandler->showWindow(visible);
    }
}

void QCefBrowserPrivate::showWindow(bool visible) {
    CEF_REQUIRE_UI_THREAD();
    if (m_hostWindow) {
        if (visible) {
            m_hostWindow->show();
        }
        else
        {
            m_hostWindow->hide();
        }
    }
}

void QCefBrowserPrivate::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    m_browserId = browser->GetIdentifier();
    emit afterCreated(m_browserId);
}

void QCefBrowserPrivate::OnClosing(CefRefPtr<CefBrowser> browser)
{
    InterlockedExchange(&m_closing, 1);
    emit closing(browser->GetIdentifier());
}

void QCefBrowserPrivate::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
	CEF_REQUIRE_UI_THREAD();
    base::AutoLock scopLock(lock);
    m_clientHandler = nullptr;
    m_hookChildWindows.clear();
    if (m_hostWindow)
        m_hostWindow->forceClose();
    emit beforeClose(browser->GetIdentifier());
}

void QCefBrowserPrivate::OnAfterCreatedSlot(int browserId) {
    qCefCoreAppPrivate()->q_ptr->browserCreated(browserId);
    
    qCefCoreAppPrivate()->addApiWindow(browserId, 
        QSharedPointer<CefApi::Window>(new CefApi::Window(nullptr, QPointer<QCefBrowser>(q_ptr))));
}

void QCefBrowserPrivate::OnClosingSlot(int browserId) {

}

void QCefBrowserPrivate::OnBeforeCloseSlot(int browserId) {
    qCefCoreAppPrivate()->q_ptr->browserClosed(browserId);
    qCefCoreAppPrivate()->removeApiWindow(browserId);
    qCefCoreAppPrivate()->removeBrowser(m_Id);
}

void QCefBrowserPrivate::OnAfterCreatedPoppup(CefRefPtr<CefBrowser> browser) {
    emit afterCreatedPoppup(browser->GetIdentifier());
}

void QCefBrowserPrivate::OnBeforeClosePoppup(CefRefPtr<CefBrowser> browser) {
    emit beforeClosePoppup(browser->GetIdentifier());
}

void QCefBrowserPrivate::afterCreatedPoppupSlot(int browserId) {
    qCefCoreAppPrivate()->addPopupBrowser(browserId);
}

void QCefBrowserPrivate::beforeClosePoppupSlot(int browserId) {

    qCefCoreAppPrivate()->removePopupBrowser(browserId);
}