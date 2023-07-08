
#include "include/wrapper/cef_helpers.h"
#include "qcefbrowserwindow.h"
#include "../qcefbrowser.h"
#include "../qcefbrowser_p.h"
#include "../qcefcoreapp.h"
#include <WindowsX.h>
#include "dpi.h"

namespace
{
// Helper function to detect mouse messages coming from emulation of touch
// events. These should be ignored.
bool IsMouseEventFromTouch(UINT message)
{
#define MOUSEEVENTF_FROMTOUCH 0xFF515700
    return (message >= WM_MOUSEFIRST) && (message <= WM_MOUSELAST)
           && (GetMessageExtraInfo() & MOUSEEVENTF_FROMTOUCH) == MOUSEEVENTF_FROMTOUCH;
}

bool IsKeyDown(WPARAM wparam)
{
    return (GetKeyState(wparam) & 0x8000) != 0;
}

int GetCefMouseModifiers(WPARAM wparam)
{
    int modifiers = 0;
    if (wparam & MK_CONTROL)
        modifiers |= EVENTFLAG_CONTROL_DOWN;
    if (wparam & MK_SHIFT)
        modifiers |= EVENTFLAG_SHIFT_DOWN;
    if (IsKeyDown(VK_MENU))
        modifiers |= EVENTFLAG_ALT_DOWN;
    if (wparam & MK_LBUTTON)
        modifiers |= EVENTFLAG_LEFT_MOUSE_BUTTON;
    if (wparam & MK_MBUTTON)
        modifiers |= EVENTFLAG_MIDDLE_MOUSE_BUTTON;
    if (wparam & MK_RBUTTON)
        modifiers |= EVENTFLAG_RIGHT_MOUSE_BUTTON;

    // Low bit set from GetKeyState indicates "toggled".
    if (::GetKeyState(VK_NUMLOCK) & 1)
        modifiers |= EVENTFLAG_NUM_LOCK_ON;
    if (::GetKeyState(VK_CAPITAL) & 1)
        modifiers |= EVENTFLAG_CAPS_LOCK_ON;
    return modifiers;
}

int GetCefKeyboardModifiers(WPARAM wparam, LPARAM lparam)
{
    int modifiers = 0;
    if (IsKeyDown(VK_SHIFT))
        modifiers |= EVENTFLAG_SHIFT_DOWN;
    if (IsKeyDown(VK_CONTROL))
        modifiers |= EVENTFLAG_CONTROL_DOWN;
    if (IsKeyDown(VK_MENU))
        modifiers |= EVENTFLAG_ALT_DOWN;

    // Low bit set from GetKeyState indicates "toggled".
    if (::GetKeyState(VK_NUMLOCK) & 1)
        modifiers |= EVENTFLAG_NUM_LOCK_ON;
    if (::GetKeyState(VK_CAPITAL) & 1)
        modifiers |= EVENTFLAG_CAPS_LOCK_ON;

    switch (wparam)
    {
    case VK_RETURN:
        if ((lparam >> 16) & KF_EXTENDED)
            modifiers |= EVENTFLAG_IS_KEY_PAD;
        break;
    case VK_INSERT:
    case VK_DELETE:
    case VK_HOME:
    case VK_END:
    case VK_PRIOR:
    case VK_NEXT:
    case VK_UP:
    case VK_DOWN:
    case VK_LEFT:
    case VK_RIGHT:
        if (!((lparam >> 16) & KF_EXTENDED))
            modifiers |= EVENTFLAG_IS_KEY_PAD;
        break;
    case VK_NUMLOCK:
    case VK_NUMPAD0:
    case VK_NUMPAD1:
    case VK_NUMPAD2:
    case VK_NUMPAD3:
    case VK_NUMPAD4:
    case VK_NUMPAD5:
    case VK_NUMPAD6:
    case VK_NUMPAD7:
    case VK_NUMPAD8:
    case VK_NUMPAD9:
    case VK_DIVIDE:
    case VK_MULTIPLY:
    case VK_SUBTRACT:
    case VK_ADD:
    case VK_DECIMAL:
    case VK_CLEAR:
        modifiers |= EVENTFLAG_IS_KEY_PAD;
        break;
    case VK_SHIFT:
        if (IsKeyDown(VK_LSHIFT))
            modifiers |= EVENTFLAG_IS_LEFT;
        else if (IsKeyDown(VK_RSHIFT))
            modifiers |= EVENTFLAG_IS_RIGHT;
        break;
    case VK_CONTROL:
        if (IsKeyDown(VK_LCONTROL))
            modifiers |= EVENTFLAG_IS_LEFT;
        else if (IsKeyDown(VK_RCONTROL))
            modifiers |= EVENTFLAG_IS_RIGHT;
        break;
    case VK_MENU:
        if (IsKeyDown(VK_LMENU))
            modifiers |= EVENTFLAG_IS_LEFT;
        else if (IsKeyDown(VK_RMENU))
            modifiers |= EVENTFLAG_IS_RIGHT;
        break;
    case VK_LWIN:
        modifiers |= EVENTFLAG_IS_LEFT;
        break;
    case VK_RWIN:
        modifiers |= EVENTFLAG_IS_RIGHT;
        break;
    }
    return modifiers;
}
} // namespace

QCefBrowserWindow::QCefBrowserWindow(QCefBrowserPrivate* browser)
    : m_browser(browser)
{

}

QCefBrowserWindow::~QCefBrowserWindow()
{
    forceClose();
}

void QCefBrowserWindow::SetTitle(const std::wstring& text)
{
    __super::setTitle(text);
}

void QCefBrowserWindow::SetAppLifeGuard()
{

}

void QCefBrowserWindow::UpdateWindow(const RECT* rc )
{

}

bool QCefBrowserWindow::onChildHitTest(WPARAM wParam, LPARAM lParam, LRESULT& result)
{
    return onHitTest(wParam, lParam, result);
}

void QCefBrowserWindow::onWindowVisbleChanged(bool visible)
{

    if (m_browser)
    {
 //       m_browser->onHostWindowVisibleChanged(visible);
    }
}

void QCefBrowserWindow::onWindowStyleChanged(windowlib::Window::WindowStyleHint)
{
}


bool QCefBrowserWindow::onHitTest(WPARAM wParam, LPARAM lParam, LRESULT& result)
{
    if (!m_browser->m_windowOptions.frameless.value())
        return false;

    POINT p = { (SHORT)LOWORD(lParam), (SHORT)HIWORD(lParam) };
    MapWindowPoints(HWND_DESKTOP, winId(), &p, 1);

    result = hitTestBorder(p);
    if (result == HTCLIENT)
    {
        result = hitTestCaption(p);
    }

    return true;
}

bool QCefBrowserWindow::onSysCommand(WPARAM wParam, LPARAM lParam, LRESULT& result)
{
    if (wParam == SC_CLOSE)
    {
        if (!isUserClosing() && m_browser->m_windowOptions.ignoreSysClose.value())
        {
            hide();
            return true;
        }
    }

    return __super::onSysCommand(wParam, lParam, result);
}

bool QCefBrowserWindow::onFocus(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result)
{
    return __super::onFocus(message, wParam, lParam, result);
}

bool QCefBrowserWindow::onClose(WPARAM wParam, LPARAM lParam, LRESULT& result)
{
    m_browser->onHostWindowWillClose();

    return __super::onClose(wParam, lParam, result);
}

bool QCefBrowserWindow::onDestory(WPARAM wParam, LPARAM lParam, LRESULT& result)
{
    m_browser->onHostWindowDestroy();
    return __super::onDestory(wParam, lParam, result);
}

bool QCefBrowserWindow::onGeomertry(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result)
{
    bool ret = __super::onGeomertry(message, wParam, lParam, result);

    if (message == WM_WINDOWPOSCHANGED)
    {
        WINDOWPOS* p = (WINDOWPOS*)lParam;
        p;

        if (!IsIconic(winId()))
        {
            m_browser->setSize();
        }

        //if ((p->flags & SWP_SHOWWINDOW) == SWP_SHOWWINDOW)
        //    m_browser->setHidden(false);
        //else if ((p->flags & SWP_HIDEWINDOW) == SWP_HIDEWINDOW)
        //    m_browser->setHidden(true);

    }
    else if (message == WM_SIZE)
    {
        if (wParam == SIZE_RESTORED
            || wParam == SIZE_MINIMIZED
            || wParam == SIZE_MAXIMIZED)
        {
            //m_browser->onWindowStateChange();
        }
    }
    return ret;
}

bool QCefBrowserWindow::onActivate(WPARAM wParam, LPARAM lParam, LRESULT& result)
{
    //m_browser->onWindowStateChange();
    return __super::onActivate(wParam, lParam, result);
}

bool QCefBrowserWindow::onDpiChange(WPARAM wParam, LPARAM lParam, LRESULT& result)
{
    bool ret = __super::onDpiChange(wParam, lParam, result);

    //m_browser->onHostWindowDpiChange(dpiScale());

    return ret;
}

QCefBrowserChildWindow::QCefBrowserChildWindow(HWND window, QCefBrowserPrivate* parent)
    : Window()
    , m_browser(parent)
{
    __super::attach(window);
}

QCefBrowserChildWindow::~QCefBrowserChildWindow()
{

}

bool QCefBrowserChildWindow::processMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result)
{
    if (message == WM_NCHITTEST)
    {
        return onHitTest(wParam, lParam, result);
    }

    return false;
}

bool QCefBrowserChildWindow::onHitTest(WPARAM wParam, LPARAM lParam, LRESULT& result)
{
    result = HTCLIENT;
    m_browser->m_hostWindow->onChildHitTest(wParam, lParam, result);
    if (result != HTCLIENT)
    {
        result = HTTRANSPARENT;
    }

    return true;
}
