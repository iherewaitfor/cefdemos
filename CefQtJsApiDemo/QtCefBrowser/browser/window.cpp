#include "window.h"
#include <windows.h>
#include <Uxtheme.h>
#include <atlstdthunk.h>
#pragma comment(lib, "atls.lib")
#include "../qcefbrowserglobal.h"
#include "dpi.h"


#ifndef WM_NCUAHDRAWCAPTION
#define WM_NCUAHDRAWCAPTION 0x00AE
#endif // WM_NCUAHDRAWCAPTION

#ifndef WM_NCUAHDRAWFRAME
#define WM_NCUAHDRAWFRAME   0x00AF
#endif // WM_NCUAHDRAWFRAME

#define WM_DPICHANGED       0x02E0

namespace {

    bool IsAfterVista()
    {
        OSVERSIONINFOW osv;
        osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFOW);
        GetVersionExW(&osv);
        return osv.dwMajorVersion >= 6;
    }

    struct WinLibary
    {
        WinLibary(const wchar_t* lib)
            : module(::LoadLibraryW(lib))
        {
        }
        ~WinLibary()
        {
            if (module)
                ::FreeLibrary(module);
        }

        HMODULE module;
    };

    static HMODULE GetModuleDWMAPI()
    {
        static WinLibary m(L"dwmapi.dll");
        return m.module;
    }

    MONITORINFO GetMonitorInformation(HWND handle)
    {
        MONITORINFO mi;
        mi.cbSize = sizeof(mi);
        HMONITOR monitor = MonitorFromWindow(handle, MONITOR_DEFAULTTONEAREST);
        GetMonitorInfo(monitor, &mi);
        return mi;
    }

    static HRESULT WINAPI pDwmExtendFrameIntoClientArea(HWND hWnd, const MARGINS* margins)
    {
        typedef HRESULT(WINAPI* funcType)(HWND hWnd, const MARGINS* margins);

        static funcType func = GetModuleDWMAPI() ? (funcType)GetProcAddress(GetModuleDWMAPI(), "DwmExtendFrameIntoClientArea") : NULL;

        return func ? func(hWnd, margins) : S_FALSE;
    }


    static HRESULT WINAPI pDwmIsCompositionEnabled(BOOL* pfEnabled)
    {
        typedef HRESULT(WINAPI* funcType)(BOOL* pfEnabled);

        static funcType func = GetModuleDWMAPI() ? (funcType)GetProcAddress(GetModuleDWMAPI(), "DwmIsCompositionEnabled") : NULL;

        return func ? func(pfEnabled) : S_FALSE;
    }

    static bool IsAeroGlassEnabled()
    {
        BOOL enabled = FALSE;
        return SUCCEEDED(pDwmIsCompositionEnabled(&enabled)) && enabled != FALSE;
    }

    static void ExtendIntoClientAll(HWND hwnd, bool shadow)
    {
        if (IsAeroGlassEnabled())
        {
            static const MARGINS shadow_state[2] = { { 0,0,0,0 },{ 0, 0, 0, 1 } };
            pDwmExtendFrameIntoClientArea(hwnd, &shadow_state[shadow ? 1 : 0]);
        }
    }

    enum Style {
        windowed = WS_OVERLAPPEDWINDOW | WS_THICKFRAME | WS_CAPTION | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPCHILDREN,
        aero_borderless = WS_POPUP | WS_THICKFRAME | WS_CAPTION | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_CLIPCHILDREN,
        basic_borderless = WS_POPUP | WS_THICKFRAME | WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_CLIPCHILDREN,
        child = WS_CHILDWINDOW | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
    };

    DWORD GetFramelessStyle() {
        return IsAeroGlassEnabled() ? (DWORD)aero_borderless : (DWORD)basic_borderless;
    }


    struct MemDC
    {
        MemDC()
        { 
            width = 0;
            height = 0;

            memDC = NULL;
            bitmap = NULL;
            originBitmap = NULL;
        }
        ~MemDC()
        {
            if (memDC)
            {
                ::DeleteObject(::SelectObject(memDC, originBitmap));
                ::DeleteObject(memDC);
            }

            memDC = NULL;
            originBitmap = NULL;
            width = 0;
            height = 0;
        }

        bool create(HDC hDC, int winWidth, int winHeight)
        {
            memDC = ::CreateCompatibleDC(hDC);
            bitmap = ::CreateCompatibleBitmap(hDC, winWidth, winHeight);
            originBitmap = ::SelectObject(memDC, bitmap);

            this->width = winWidth;
            this->height = winHeight;
            return !!memDC;
        }

        operator HDC() { return memDC; }

        int width;
        int height;

        HDC memDC;
        HBITMAP bitmap;
        HGDIOBJ originBitmap;
    };
}


namespace windowlib
{

    class ATLThunk
    {
    public:
        ATLThunk()
        {
            thunk_ = NULL;
        }

        ~ATLThunk()
        {
            if (thunk_)
                delete static_cast<ATL::_stdcallthunk*>(thunk_);
        }

        bool init(void* proc, void* pThis)
        {
            if (thunk_)
                return false;

            thunk_ = new ATL::_stdcallthunk;
            return static_cast<ATL::_stdcallthunk*>(thunk_)->Init((DWORD_PTR)proc, pThis) != FALSE;
        }

        void* address() const
        {
            if (thunk_)
                return static_cast<ATL::_stdcallthunk*>(thunk_)->GetCodeAddress();

            return NULL;
        }

        void* thunk_;
    };

    struct WindowStyle
    {
        WindowStyle()
        {
            resizable = true;
            frameless = true;
            areoShadow = true;
            showInTaskbar = true;
        }

        bool resizable;
        bool frameless;
        bool areoShadow;
        bool showInTaskbar;
    };

    class WindowPrivate
    {
    public:
        Window* q_ptr;
        std::wstring title_;
        HWND hWnd_ ;
        bool isUserClose_ ;
        HBRUSH backgroundBrush_ ;
        WindowStyle windowStyle_;
        RECT rect_;
        MemDC memDC_;

        int minWidth;
        int minHeight;
        int maxWidth;
        int maxHeight;
        int ncBorder;
        std::vector<RECT> captionAreas_;
        HRGN draggable_region_;

        bool isMouseOver_;

        bool isFullScreen_;
        struct SaveInfo {
            DWORD style;
            DWORD exStyle;

            bool maximized;
            RECT rect;
            RECT restoreNormalRect_;
        };
        SaveInfo restoreWindowInfo_;

        WNDPROC prevProc_;
        ATLThunk winProcThunk_;

        float dpiScale_;

        WindowPrivate(Window* q) : q_ptr(q)
        {
            minWidth = 0;
            minHeight = 0;
            maxWidth = 0;
            maxHeight = 0;
            ncBorder = 8;
            dpiScale_ = DPI().isHIDPIEnabled() ? DPI().getDPIScale() : 1;

            draggable_region_ = ::CreateRectRgn(0, 0, 0, 0);
            isMouseOver_ = false;
            isFullScreen_ = false;

            prevProc_ = NULL;

            hWnd_ = NULL;
            isUserClose_ = false;
            backgroundBrush_ = NULL;

            RECT workArea;
            SystemParametersInfo(SPI_GETWORKAREA, 0, &workArea, 0);
            const int width = 800;
            const int height = 600;
            rect_.left = workArea.left + (workArea.right - workArea.left - width) / 2;
            rect_.top = workArea.top + (workArea.bottom - workArea.top - height) / 2;
            rect_.right = rect_.left + width;
            rect_.bottom = rect_.top + height;
        }

        ~WindowPrivate()
        {
            if (hWnd_ && prevProc_)
            {
                SetWindowLongPtr(hWnd_, GWLP_WNDPROC, (LONG_PTR)prevProc_);
            }
            if (backgroundBrush_)
            {
                DeleteObject(backgroundBrush_);
            }
            DeleteObject(draggable_region_);
        }

        static const wchar_t* cls()
        {
            static const wchar_t* cls = L"QCefWindow";
            WNDCLASSEX wc = { 0 };
            wc.cbSize = sizeof(WNDCLASSEX);
            wc.style = CS_HREDRAW | CS_VREDRAW;
            wc.lpfnWndProc = DefWindowProc;
            wc.hInstance = GetModuleHandle(NULL);
            wc.lpszClassName = cls;
            wc.hCursor = LoadCursor(NULL, IDC_ARROW);
            //wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
            wc.hbrBackground = ::CreateSolidBrush(RGB(36, 37, 50) & 0x00FFFFFF);// 
            return !!RegisterClassEx(&wc) ? cls : NULL;
        }

        static const wchar_t* windowClass()
        {
            static const wchar_t* className = cls();
            return className;
        }


        void create(HWND parent = NULL, bool childWindow = false, DWORD styles = 0, DWORD exstyles = 0)
        {
            if (hWnd_ == NULL)
            {
                DWORD style = styles ? styles : childWindow ? (DWORD)child : (DWORD)windowed;
                DWORD styleEx = exstyles;

                if (!childWindow)
                {
                    if (!windowStyle_.resizable)
                    {
                        style &= ~(WS_MAXIMIZEBOX);
                    }

                    if (!windowStyle_.showInTaskbar)
                    {
                        styleEx |= WS_EX_TOOLWINDOW;
                    }
                }

                HWND wnd = CreateWindowExW(styleEx, WindowPrivate::windowClass(), title_.c_str(), style,
                    rect_.left, rect_.top, rect_.right - rect_.left, rect_.bottom - rect_.top,
                    parent, NULL, GetModuleHandle(NULL), NULL);

                attach(wnd);

                if (!childWindow)
                {
                    if (windowStyle_.frameless)
                        setFrameless();
                }
                
            }
        }

        void attach(HWND hWnd)
        {
            assert(hWnd_ == NULL);

            hWnd_ = hWnd;

            winProcThunk_.init(reinterpret_cast<void*>(&WindowPrivate::windowProc), this);
            prevProc_ = (WNDPROC)::SetWindowLongPtr(hWnd_, GWLP_WNDPROC, (LONG_PTR)winProcThunk_.address());

            GetWindowRect(hWnd, &rect_);
        }

        void SetAreoGlass(bool areoShadow)
        {
            ExtendIntoClientAll(hWnd_, areoShadow);
            ::SetWindowPos(hWnd_, NULL, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        }

        void setFrameless()
        {
            DWORD new_style = (windowStyle_.frameless) ? GetFramelessStyle() : (DWORD)windowed;

            if (!windowStyle_.resizable)
            {
                new_style &= ~(WS_MAXIMIZEBOX);
            }

            DWORD old_style = ::GetWindowLongPtrW(hWnd_, GWL_STYLE);

            if (new_style != old_style) {

                ::SetWindowLongPtrW(hWnd_, GWL_STYLE, new_style);
                SetAreoGlass(windowStyle_.areoShadow);
            }
        }

        void setResizable()
        {
            DWORD old_style = ::GetWindowLongPtrW(hWnd_, GWL_STYLE);
            DWORD new_style = old_style;
            if (windowStyle_.resizable)
            {
                new_style |= WS_MAXIMIZEBOX;
            }
            else
            {
                new_style &= ~(WS_MAXIMIZEBOX);
            }

            if (new_style != old_style)
            {
                ::SetWindowLongPtrW(hWnd_, GWL_STYLE, new_style);
            }
        }

        void setShowInTaskbar()
        {
            DWORD old_style = ::GetWindowLongW(hWnd_, GWL_EXSTYLE);
            DWORD new_style = windowStyle_.showInTaskbar ?
                old_style | WS_EX_TOOLWINDOW : old_style & ~WS_EX_TOOLWINDOW;

            if (new_style != old_style)
            {
                ::SetWindowLongPtrW(hWnd_, GWL_EXSTYLE, new_style);
            }
        }

        void setRect(RECT v)
        {
            if (hWnd_)
                SetWindowPos(hWnd_, NULL, v.left, v.top, v.right - v.left, v.bottom - v.top,
                             SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

            rect_ = v;

            restoreWindowInfo_.restoreNormalRect_ = v;
        }

        void setRect(int x, int y, int width, int height)
        {
            RECT rc = { x, y, x + width, y + height };
            setRect(rc);
        }

        void setClientRect(RECT rc)
        {
            if (hWnd_)
            {
                RECT rcWin = { 0 };
                RECT rcClient = { 0 };
                ::GetWindowRect(hWnd_, &rcWin);
                ::GetClientRect(hWnd_, &rcClient);
                SIZE frameSize = { 0 };
                frameSize.cx = (rcWin.right - rcWin.left) - (rcClient.right - rcClient.left);
                frameSize.cy = (rcWin.bottom - rcWin.top) - (rcClient.bottom - rcClient.top);
                setRect(rc.left, rc.top, rc.right - rc.left + frameSize.cx, rc.bottom - rc.top + frameSize.cy);
            }
        }

        static LRESULT CALLBACK windowProc(WindowPrivate* pThis, UINT uMsg, WPARAM wParam, LPARAM lParam)
        {
            return pThis->WindowProc(uMsg, wParam, lParam);
        }

        LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
        {
            LRESULT lResult = 0;

            if (q_ptr->processMessage(uMsg, wParam, lParam, lResult))
            {
                return lResult;
            }

            return prevProc_(hWnd_, uMsg, wParam, lParam);
        }
    };


    Window::Window()
        : d_ptr(new WindowPrivate(this))
    {

    }

    Window::~Window()
    {
        //do not close
        delete d_ptr;
    }

    void Window::attach(HWND hWnd)
    {
        d_ptr->attach(hWnd);
    }

    void Window::create(HWND hWndParent /*= NULL*/, DWORD styles, DWORD exstyles)
    {
        d_ptr->create(hWndParent, false, styles, exstyles);
    }

    void Window::createAsChild(HWND hWndParent)
    {
        POINT pt = { 0, 0 };
        move(pt);
        d_ptr->create(hWndParent, true);
    }

    bool Window::isVisible() const
    {
        return IsWindow(winId()) && IsWindowVisible(winId());
    }

    void Window::setTitle(const std::wstring& text)
    {
        if (winId())
        {
            ::SetWindowTextW(winId(), text.c_str());
        }
    }

    void Window::showActive()
    {
        if (IsWindow(winId()))
        {
            ::ShowWindow(winId(), SW_SHOW);
            if (IsIconic(winId()))
            {
                restore();
            }
            ::SetActiveWindow(winId());
        }
    }

    void Window::show()
    {
        if (IsWindow(winId()))
            ::ShowWindow(winId(), SW_SHOW);
    }

    void Window::showNoActive()
    {
        if (IsWindow(winId()))
            ::ShowWindow(winId(), SW_SHOWNOACTIVATE);
    }

    void Window::hide()
    {
        if (IsWindow(winId()))
            ::ShowWindow(winId(), SW_HIDE);
    }

    void Window::maximize()
    {
        if (IsWindow(winId()))
            ShowWindow(winId(), SW_SHOWMAXIMIZED);
    }

    void Window::minimize()
    {
        if (IsWindow(winId()))
        {
            ShowWindow(winId(), SW_SHOWMINIMIZED);
            if (!d_ptr->windowStyle_.showInTaskbar)
            {
                ShowWindow(winId(), SW_HIDE);
            }
        }

    }

    void Window::restore()
    {
        if (IsWindow(winId()))
            ShowWindow(winId(), SW_RESTORE);
    }

    void Window::maximizeOrRestore()
    {
        if (IsWindow(winId()))
        {
            if (IsZoomed(winId()))
                restore();
            else
                maximize();
        }
    }

    void Window::close()
    {
        if (IsWindow(winId()))
        {
            ::SendMessage(winId(), WM_SYSCOMMAND, SC_CLOSE, 0);
        }
    }

    void Window::forceClose()
    {
        if (IsWindow(winId()))
        {
            d_ptr->isUserClose_ = true;
            ::SendMessage(winId(), WM_SYSCOMMAND, SC_CLOSE, 0);
            if (IsWindow(winId()))
                ::DestroyWindow(winId());
            d_ptr->hWnd_ = NULL;
        }
    }

    void Window::update(const RECT* rc /*= NULL*/)
    {
        ::InvalidateRect(winId(), rc, TRUE);
        ::UpdateWindow(winId());
    }

    void Window::bringToTop()
    {
        if (IsWindow(winId()))
            ::BringWindowToTop(winId());
    }


    void Window::flashTaskbar()
    {
        if (IsWindow(winId()))
        {
            FLASHWINFO flashInfo = { sizeof(flashInfo), winId(), FLASHW_TRAY | FLASHW_TIMERNOFG , 5, 0 };

            ::FlashWindowEx(&flashInfo);
        }
    }

    void Window::stopFlashTaskbar()
    {
        if (IsWindow(winId()))
        {
            FLASHWINFO flashInfo = { sizeof(flashInfo), winId(), FLASHW_STOP, 0, 0 };
            ::FlashWindowEx(&flashInfo);
        }
    }

    void Window::setBackgourndColor(COLORREF color)
    {
        d_ptr->backgroundBrush_ = ::CreateSolidBrush(color & 0x00FFFFFF);
    }

    void Window::setFrameless(bool v, bool areoShadow)
    {
        if (d_ptr->windowStyle_.frameless != v
            || d_ptr->windowStyle_.areoShadow != areoShadow)
        {
            d_ptr->windowStyle_.frameless = v;
            d_ptr->windowStyle_.areoShadow = areoShadow;

            if (IsWindow(winId()))
                d_ptr->setFrameless();

            onWindowStyleChanged(frameless);
        }

    }

    void Window::setShowInTaskbar(bool v)
    {
        if (d_ptr->windowStyle_.showInTaskbar != v)
        {
            d_ptr->windowStyle_.showInTaskbar = v;

            if (IsWindow(winId()))
                d_ptr->setShowInTaskbar();

            onWindowStyleChanged(showInTaskbar);
        }
    }

    bool Window::isResizable() const
    {
        return d_ptr->windowStyle_.resizable;
    }

    void Window::setResizable(bool v)
    {
        if (d_ptr->windowStyle_.resizable != v)
        {
            d_ptr->windowStyle_.resizable = v;

            if (IsWindow(winId()))
            {
                d_ptr->setResizable();
            }

            onWindowStyleChanged(resizable);
        }

    }

    void Window::setTopMost(bool v)
    {
        ::SetWindowPos(winId(), v ? HWND_TOPMOST : HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOOWNERZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE);
    }

    bool Window::isTopMost() const
    {
        if (winId())
        {
            DWORD dwExStyle = ::GetWindowLong(winId(), GWL_EXSTYLE);

            if ((dwExStyle & WS_EX_TOPMOST) != 0)
            {
                return true;
            }
        }
        return false;
    }

    POINT Window::pos() const
    {
        POINT pt = { d_ptr->rect_.left, d_ptr->rect_.top };
        return pt;
    }

    void Window::move(const POINT& v)
    {
        RECT rc = { v.x, v.y, v.x + width(), v.y + height() };
        setRect(rc);
    }

    void Window::moveCenter()
    {
        RECT windowRect = rect();

        HWND parent = GetParent(winId());
        RECT parentRect;
        if ((parent == HWND_DESKTOP
            || !IsWindowVisible(parent)
            || IsIconic(parent))
            && (GetWindowLongPtr(winId(), GWL_STYLE) & WS_CHILD) == 0
            )
        {
            MONITORINFO info = GetMonitorInformation(winId());
            int workWidth = info.rcWork.right - info.rcWork.left;
            int workHeight = info.rcWork.bottom - info.rcWork.top;
            POINT p = { 0 };
            p.x = info.rcWork.left + (workWidth - (windowRect.right - windowRect.left)) / 2;
            p.y = info.rcWork.top + (workHeight - (windowRect.bottom - windowRect.top)) / 2;

            if (p.x < info.rcWork.left)
            {
                p.x = info.rcWork.left;
            }
            if (p.y < info.rcWork.top)
            {
                p.y = info.rcWork.top;
            }
            move(p);
            return;
        }
        else
        {
            if ((GetWindowLongPtr(winId(), GWL_STYLE) & WS_CHILD))
                ::GetClientRect(parent, &parentRect);
            else
                ::GetWindowRect(parent, &parentRect);
        }

        POINT p = { parentRect.right - parentRect.left, parentRect.bottom - parentRect.top };

        p.x = (p.x - windowRect.right + windowRect.left) / 2;
        p.y = (p.y - windowRect.bottom + windowRect.top) / 2;

        if ((GetWindowLongPtr(winId(), GWL_STYLE) & WS_CHILD) == 0)
        {
            p.x += parentRect.left;
            p.y += parentRect.top;
        }

        if (p.x < 0) p.x = 0;
        if (p.y < 0) p.y = 0;

        move(p);
    }

    SIZE Window::size() const
    {
        SIZE sz = { d_ptr->rect_.right - d_ptr->rect_.left, d_ptr->rect_.bottom - d_ptr->rect_.top };
        return sz;
    }

    void Window::resize(const SIZE& sz)
    {
        RECT rc = { d_ptr->rect_.left, d_ptr->rect_.top, d_ptr->rect_.left + sz.cx, d_ptr->rect_.top + sz.cy };
        setRect(rc);
    }

    void Window::setClientSize(const SIZE& sz)
    {
        RECT rc = { 0, 0, sz.cx, sz.cy };

        DWORD style = ::GetWindowLongPtrW(winId(), GWL_STYLE);
        DWORD styleEx = ::GetWindowLongPtrW(winId(), GWL_EXSTYLE);

        AdjustWindowRectEx(&rc, style, FALSE, styleEx);
        SIZE windowSize = { rc.right - rc.left, rc.bottom - rc.top };
        resize(windowSize);
    }

    RECT Window::rect() const
    {
        return d_ptr->rect_;
    }

    RECT Window::clientRect() const
    {
        RECT rc = {};
        GetClientRect(winId(), &rc);

        return rc;
    }

    void Window::setRect(RECT rc)
    {
        d_ptr->setRect(rc);
    }

    void Window::setClientRect(RECT rc)
    {
        d_ptr->setClientRect(rc);
    }

    int Window::width() const
    {
        return d_ptr->rect_.right - d_ptr->rect_.left;
    }

    void Window::setWidth(int w)
    {
        RECT rc = { d_ptr->rect_.left, d_ptr->rect_.top, d_ptr->rect_.left + width(), d_ptr->rect_.bottom };
        setRect(rc);
    }

    int Window::height() const
    {
        return d_ptr->rect_.bottom - d_ptr->rect_.top;
    }

    void Window::setHeight(int h)
    {
        RECT rc = { d_ptr->rect_.left, d_ptr->rect_.top, d_ptr->rect_.right, d_ptr->rect_.top + h };
        setRect(rc);
    }

    int Window::minWidth() const
    {
        return d_ptr->minWidth;
    }

    void Window::setMinWidth(int w)
    {
        d_ptr->minWidth = w;
    }

    int Window::minHeight() const
    {
        return d_ptr->minHeight;
    }

    void Window::setMinHeight(int h)
    {
        d_ptr->minHeight = h;
    }

    int Window::maxWidth() const
    {
        return d_ptr->maxWidth;
    }

    void Window::setMaxWidth(int w)
    {
        d_ptr->maxWidth = w;
    }

    int Window::maxHeight() const
    {
        return d_ptr->maxHeight;
    }

    void Window::setMaxHeight(int h)
    {
        d_ptr->maxHeight = h;
    }

    void Window::setNcBorderWidth(int w)
    {
        d_ptr->ncBorder = w;
    }

    void Window::setCaptionArea(const std::vector<RECT>& v)    {
        d_ptr->captionAreas_ = v;
        ::SetRectRgn(d_ptr->draggable_region_, 0, 0, 0, 0);

        // Determine new draggable region.
        for (DWORD i = 0; i != v.size(); ++i)
        {
            HRGN region = ::CreateRectRgn(v[i].left, v[i].top,
                v[i].right,
                v[i].bottom);
            ::CombineRgn(d_ptr->draggable_region_, d_ptr->draggable_region_, region, RGN_OR);
            ::DeleteObject(region);
        }
    }

    void Window::enterFullScreen()
    {
        if (!IsWindow(winId()))
        {
            return;
        }

        if (!d_ptr->isFullScreen_)
        {
            d_ptr->isFullScreen_ = true;

            d_ptr->restoreWindowInfo_.style = GetWindowLong(winId(), GWL_STYLE);
            d_ptr->restoreWindowInfo_.exStyle = GetWindowLong(winId(), GWL_EXSTYLE);
            d_ptr->restoreWindowInfo_.maximized = !!IsZoomed(winId());
            d_ptr->restoreWindowInfo_.rect = d_ptr->rect_;

            SetWindowLong(winId(), GWL_STYLE,
                d_ptr->restoreWindowInfo_.style & ~(WS_CAPTION | WS_THICKFRAME));
            SetWindowLong(winId(), GWL_EXSTYLE,
                d_ptr->restoreWindowInfo_.exStyle & ~(WS_EX_DLGMODALFRAME |
                    WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE));

            MONITORINFO monitor_info;
            monitor_info.cbSize = sizeof(monitor_info);
            GetMonitorInfo(MonitorFromWindow(winId(), MONITOR_DEFAULTTONEAREST),
                &monitor_info);

            if (d_ptr->restoreWindowInfo_.maximized)
            {
                ShowWindow(winId(), SW_SHOWNORMAL);
            }

            SetWindowPos(winId(), NULL, monitor_info.rcMonitor.left, monitor_info.rcMonitor.top,
                monitor_info.rcMonitor.right - monitor_info.rcMonitor.left, monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top,
                SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);

        }
    }

    void Window::leaveFullScreen()
    {
        if (d_ptr->isFullScreen_)
        {
            d_ptr->isFullScreen_ = false;

            SetWindowLong(winId(), GWL_STYLE, d_ptr->restoreWindowInfo_.style);
            SetWindowLong(winId(), GWL_EXSTYLE, d_ptr->restoreWindowInfo_.exStyle);

            if (d_ptr->restoreWindowInfo_.maximized)
            {
                WINDOWPLACEMENT placement = { 0 };
                placement.length = sizeof(WINDOWPLACEMENT);
                placement.showCmd = SW_SHOWMAXIMIZED;
                placement.rcNormalPosition = d_ptr->restoreWindowInfo_.restoreNormalRect_;
                SetWindowPlacement(winId(), &placement);
            }
            else
            {
                SetWindowPos(winId(), NULL, d_ptr->restoreWindowInfo_.rect.left, d_ptr->restoreWindowInfo_.rect.top, d_ptr->restoreWindowInfo_.rect.right - d_ptr->restoreWindowInfo_.rect.left,
                    d_ptr->restoreWindowInfo_.rect.bottom - d_ptr->restoreWindowInfo_.rect.top,
                    SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
            }
        }
    }

    bool Window::isFullScreen() const
    {
        return d_ptr->isFullScreen_;
    }

    float Window::dpiScale() const
    {
        return d_ptr->dpiScale_;
    }

    bool Window::isUserClosing() const
    {
        return d_ptr->isUserClose_;
    }

    bool Window::processMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result)
    {
        switch (message)
        {
        case WM_NCDESTROY:
            d_ptr->hWnd_ = NULL;
            break;
        case WM_NCACTIVATE:
            return onNCActivate(wParam, lParam, result);
        case WM_NCCALCSIZE:
            return onNCCalcSize(wParam, lParam, result);
        case WM_SHOWWINDOW:
            onWindowVisbleChanged(wParam == TRUE);
            break;
        case WM_GETMINMAXINFO:
            return onMinMaxInfo(wParam, lParam, result);
        case WM_SYSCOMMAND:
            return onSysCommand(wParam, lParam, result);
        case WM_ACTIVATE:
            return onActivate(wParam, lParam, result);
        case WM_CLOSE:
            return onClose(wParam, lParam, result);
        case WM_DESTROY:
            return onDestory(wParam, lParam, result);
        case WM_NCHITTEST:
            return onHitTest(wParam, lParam, result);
        case WM_SETFOCUS:
        case WM_KILLFOCUS:
            return onFocus(message, wParam, lParam, result);
        case WM_SIZE:
        case WM_MOVING:
        case WM_MOVE:
        case WM_WINDOWPOSCHANGING:
        case WM_WINDOWPOSCHANGED:
            return onGeomertry(message, wParam, lParam, result);

        case WM_NCUAHDRAWCAPTION:
        case WM_NCUAHDRAWFRAME:
            if (d_ptr->windowStyle_.frameless)
            {
                result = 0;
                return true;
            }
            break;
        case WM_PAINT:
            return onPaint(wParam, lParam, result);
        case WM_NCPAINT:
            return onNCPaint(wParam, lParam, result);
        case WM_ERASEBKGND:
            return onEraseBKGND(wParam, lParam, result);
        case WM_SETCURSOR:
            return onSetCursor(wParam, lParam, result);
        case WM_DPICHANGED:

        default:
            break;
        }

        if ((message >= WM_MOUSEFIRST && message <= WM_MOUSELAST) 
            || message == WM_MOUSELEAVE 
            || message == WM_MOUSEHOVER)
        {
            return onMouseMessage(message, wParam, lParam, result);
        }
        else if (message >= WM_NCMOUSEMOVE && message <= WM_NCMBUTTONDBLCLK)
        {
            return onNCMouseMessage(message, wParam, lParam, result);
        }
        else if (message >= WM_KEYFIRST && message <= WM_KEYLAST)
        {
            return onKeyMessage(message, wParam, lParam, result);
        }
        return false;
    }

    bool Window::onNCActivate(WPARAM wParam, LPARAM lParam, LRESULT& result)
    {
        result = TRUE;
        return true;
    }

    bool Window::onNCCalcSize(WPARAM wParam, LPARAM lParam, LRESULT& result)
    {
        if (wParam == TRUE && d_ptr->windowStyle_.frameless)
        {
            if (!IsZoomed(winId()))
            {
                return true;
            }

            NCCALCSIZE_PARAMS* pParams = reinterpret_cast<NCCALCSIZE_PARAMS*>(lParam);
            MONITORINFO mi = GetMonitorInformation(winId());
            pParams->rgrc[0] = mi.rcWork;
            pParams->rgrc[1] = pParams->rgrc[0];

            result = 0;
            return true;
        }
        return false;
    }

    bool Window::onMinMaxInfo(WPARAM wParam, LPARAM lParam, LRESULT& result)
    {
        MINMAXINFO* mmi = (MINMAXINFO*)lParam;
        if (d_ptr->minWidth > 0)
            mmi->ptMinTrackSize.x = d_ptr->minWidth;
        if (d_ptr->minHeight > 0)
            mmi->ptMinTrackSize.y = d_ptr->minHeight;
        if (d_ptr->maxWidth > 0)
            mmi->ptMaxTrackSize.x = d_ptr->maxWidth;
        if (d_ptr->maxHeight > 0)
            mmi->ptMaxTrackSize.y = d_ptr->maxHeight;

        if (d_ptr->windowStyle_.frameless)
        {
            MONITORINFO mi = GetMonitorInformation(winId());
            const RECT& rcWork = mi.rcWork;
            const RECT& rcMonitor = mi.rcMonitor;
            mmi->ptMaxPosition.x = rcWork.left - rcMonitor.left;
            mmi->ptMaxPosition.y = rcWork.top - rcMonitor.top;
            mmi->ptMaxSize.x = rcWork.right - rcWork.left;
            mmi->ptMaxSize.y = rcWork.bottom - rcWork.top;
        }
        result = 0;
        return true;
    }

    bool Window::onHitTest(WPARAM wParam, LPARAM lParam, LRESULT& result)
    {
        if (!d_ptr->windowStyle_.frameless)
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

    LRESULT Window::hitTestBorder(POINT p)
    {
        LRESULT result = HTNOWHERE;

        if (IsZoomed(winId()))
            return HTCLIENT;

        static const LRESULT results[3][3] =
        {
            { HTTOPLEFT, HTTOP, HTTOPRIGHT },
            { HTLEFT, HTCLIENT, HTRIGHT },
            { HTBOTTOMLEFT, HTBOTTOM, HTBOTTOMRIGHT },
        };

        const int borderWidth = d_ptr->ncBorder;

        const int x = p.x < borderWidth ? 0 : (p.x >= width() - borderWidth ? 2 : 1);
        const int y = p.y < borderWidth ? 0 : (p.y >= height() - borderWidth ? 2 : 1);
        if (results[y][x] != HTCLIENT)
        {
            result = d_ptr->windowStyle_.resizable ? results[y][x] : HTBORDER;
        }
        else
        {
            result = results[y][x];
        }

        return result;
    }

    LRESULT Window::hitTestCaption(POINT point)
    {
        LRESULT result = HTNOWHERE;
        /*auto captions = d_ptr->captionAreas_;
        const bool isCaption = std::any_of(captions.cbegin(), captions.cend(), [&](auto& rc)
            {
                return p.x >= rc.left && p.x < rc.right&& p.y >= rc.top && p.y < rc.bottom;
            });
        */

        if (::PtInRegion(d_ptr->draggable_region_, point.x, point.y)) {
            result = HTCAPTION;
        }
        else
        {
            result = HTCLIENT;
        }
        return result;
    }

    bool Window::onActivate(WPARAM wParam, LPARAM lParam, LRESULT& result)
    {
        return false;
    }

    bool Window::onSysCommand(WPARAM wParam, LPARAM lParam, LRESULT& result)
    {
        if (wParam == SC_CLOSE)
        {
            SendMessage(winId(), WM_CLOSE, NULL, NULL);
            return true;
        }
        return false;
    }

    bool Window::onClose(WPARAM wParam, LPARAM lParam, LRESULT& result)
    {
        return false;
    }

    bool Window::onDestory(WPARAM wParam, LPARAM lParam, LRESULT& result)
    {
        d_ptr->hWnd_ = NULL;

        return false;
    }

    bool Window::onPaint(WPARAM wParam, LPARAM lParam, LRESULT& result)
    {
        PAINTSTRUCT ps = {};
        HDC hDC = BeginPaint(winId(), &ps);

        RECT rect = clientRect();
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;
        if (d_ptr->memDC_.width != width || d_ptr->memDC_.height != height)
        {
            d_ptr->memDC_.~MemDC();
            d_ptr->memDC_.create(hDC, width, height);
        }

        paintBackground(d_ptr->memDC_);
        paint(d_ptr->memDC_);

        ::BitBlt(hDC, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top, d_ptr->memDC_, ps.rcPaint.left, ps.rcPaint.top, SRCCOPY);

        EndPaint(winId(), &ps);
        return true;
    }

    void Window::paint(HDC hDC)
    {

    }

    void Window::paintBackground(HDC hDC)
    {
        if (d_ptr->backgroundBrush_)
        {
            RECT rc;
            ::GetClientRect(winId(), &rc);
            ::FillRect(hDC, &rc, d_ptr->backgroundBrush_);
        }
    }

    bool Window::onNCPaint(WPARAM wParam, LPARAM lParam, LRESULT& result)
    {
        return (d_ptr->windowStyle_.frameless && !IsAeroGlassEnabled());
    }

    bool Window::onEraseBKGND(WPARAM wParam, LPARAM lParam, LRESULT& result)
    {
        if (d_ptr->backgroundBrush_)
        {
            result = TRUE;
            return true;
        }
        else
        {
            return false;
        }
    }

    bool Window::onFocus(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result)
    {
        return false;
    }

    HWND Window::winId() const
    {
        return d_ptr->hWnd_;
    }

    bool Window::onGeomertry(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result)
    {
        switch (message)
        {
        case WM_WINDOWPOSCHANGED:
        {
            GetWindowRect(winId(), &d_ptr->rect_);
            result = DefWindowProcW(winId(), WM_WINDOWPOSCHANGED, wParam, lParam);
            return true;
        }
        break;
        break;
        }
        return false;
    }

    bool Window::onMouseMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
    {
        switch (message)
        {
        case WM_CAPTURECHANGED:
            d_ptr->isMouseOver_ = false;
            break;
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        {
            ::SetCapture(winId());
            ::SetFocus(winId());
        }
        break;
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        {
            if (GetCapture() == winId())
                ReleaseCapture();
        }
        break;
        case WM_MOUSEMOVE:
            if (!d_ptr->isMouseOver_)
            {
                TRACKMOUSEEVENT tme;
                tme.cbSize = sizeof(tme);
                tme.dwFlags = TME_LEAVE;
                tme.hwndTrack = winId();
                tme.dwHoverTime = 0;

                ::TrackMouseEvent(&tme);
                d_ptr->isMouseOver_ = true;
            }
            break;
        case WM_MOUSELEAVE:
            d_ptr->isMouseOver_ = false;
            break;
        default:
            break;
        }

        return false;
    }

    bool Window::onNCMouseMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
    {
        return false;
    }

    bool Window::onKeyMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult)
    {
        return false;
    }

    bool Window::onSetCursor(WPARAM wParam, LPARAM lParam, LRESULT& result)
    {
        return false;
    }

    bool Window::onDpiChange(WPARAM wParam, LPARAM lParam, LRESULT& result)
    {
        if (LOWORD(wParam) != HIWORD(wParam))
        {
            return false;
        }

        d_ptr->dpiScale_ = static_cast<float>(LOWORD(wParam)) / 96.0f;

        // Suggested size and position of the current window scaled for the new DPI.
        const RECT* rect = reinterpret_cast<RECT*>(lParam);
        d_ptr->setRect(*rect);

        return false;
    }

    void Window::onWindowVisbleChanged(bool visible)
    {

    }

    void Window::onWindowStyleChanged(WindowStyleHint style)
    {

    }

}
