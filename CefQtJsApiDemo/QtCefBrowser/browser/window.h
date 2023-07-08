#pragma once
#include "../qcefbrowserglobal.h"
#include <QtCore>
#include <Windows.h>

namespace windowlib {

    class WindowPrivate;
    class QCEFBROWSER_DECL_EXPORT  Window
    {
        Q_DISABLE_COPY(Window);

        QCEFBROWSER_DECLARE_PRIVATE(Window);

    public:
        Window();
        virtual ~Window();

        void attach(HWND hWnd);
        void create(HWND hWndParent = NULL, DWORD styles = 0, DWORD exStyles = 0);
        void createAsChild(HWND hWndParent);

        HWND winId() const;
        bool isVisible() const;

        void setTitle(const std::wstring& text);

        void showActive();

        void show();
        void showNoActive();
        void hide();
        void maximize();
        void minimize();
        void restore();
        void maximizeOrRestore();
        void close();
        void forceClose();

        void update(const RECT* rc = NULL);
        void bringToTop();
        void flashTaskbar();
        void stopFlashTaskbar();

        void setBackgourndColor(COLORREF color);
        void setFrameless(bool v, bool areoShadow = true);
        void setShowInTaskbar(bool v);

        bool isResizable() const;
        void setResizable(bool v);

        void setTopMost(bool);
        bool isTopMost() const;

        POINT pos() const;
        void move(const POINT& v);

        void moveCenter();

        SIZE size() const;
        void resize(const SIZE& sz);
        void setClientSize(const SIZE& sz);

        RECT rect() const;
        RECT clientRect() const;
        void setRect(RECT);
        void setClientRect(RECT);

        int width() const;
        void setWidth(int w);

        int height() const;
        void setHeight(int h);

        int minWidth() const;
        void setMinWidth(int w);

        int minHeight() const;
        void setMinHeight(int w);

        int maxWidth() const;
        void setMaxWidth(int w);

        int maxHeight() const;
        void setMaxHeight(int h);

        void setNcBorderWidth(int w);
        void setCaptionArea(const std::vector<RECT>& v);

        void enterFullScreen();
        void leaveFullScreen();
        bool isFullScreen() const;

        float dpiScale() const;

        LRESULT hitTestBorder(POINT windowPoint);
        LRESULT hitTestCaption(POINT windowPoint);
    protected:
        bool isUserClosing() const;
        virtual void paint(HDC);
        virtual void paintBackground(HDC);

        virtual bool processMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result);
        virtual bool onNCActivate(WPARAM wParam, LPARAM lParam, LRESULT& result);
        virtual bool onNCCalcSize(WPARAM wParam, LPARAM lParam, LRESULT& result);
        virtual bool onMinMaxInfo(WPARAM wParam, LPARAM lParam, LRESULT& result);
        virtual bool onHitTest(WPARAM wParam, LPARAM lParam, LRESULT& result);


        virtual bool onActivate(WPARAM wParam, LPARAM lParam, LRESULT& result);
        virtual bool onSysCommand(WPARAM wParam, LPARAM lParam, LRESULT& result);
        virtual bool onClose(WPARAM wParam, LPARAM lParam, LRESULT& result);
        virtual bool onDestory(WPARAM wParam, LPARAM lParam, LRESULT& result);
        virtual bool onPaint(WPARAM wParam, LPARAM lParam, LRESULT& result);
        virtual bool onNCPaint(WPARAM wParam, LPARAM lParam, LRESULT& result);
        virtual bool onEraseBKGND(WPARAM wParam, LPARAM lParam, LRESULT& result);
        virtual bool onFocus(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result);
        virtual bool onGeomertry(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& result);

        virtual bool onMouseMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult);

        virtual bool onNCMouseMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult);
        virtual bool onKeyMessage(UINT message, WPARAM wParam, LPARAM lParam, LRESULT& lResult);

        virtual bool onSetCursor(WPARAM wParam, LPARAM lParam, LRESULT& result);

        virtual bool onDpiChange(WPARAM wParam, LPARAM lParam, LRESULT& result);

    protected:
        enum WindowStyleHint
        {
            resizable = 1,
            frameless,
            areoShadow,
            showInTaskbar
        };

        virtual void onWindowVisbleChanged(bool visible);
        virtual void onWindowStyleChanged(WindowStyleHint style);

    };
}

